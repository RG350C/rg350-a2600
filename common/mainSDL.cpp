//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2005 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: mainSDL.cxx,v 1.65 2006/03/22 21:13:36 stephena Exp $
//============================================================================

#include <sstream>

#include <SDL/SDL.h>
#include "psp_sdl.h"

#include "bspf.hxx"
#include "Console.hxx"
#include "Event.hxx"
#include "Deserializer.hxx"
#include "EventHandler.hxx"
#include "FrameBuffer.hxx"
#include "PropsSet.hxx"
#include "Sound.hxx"
#include "System.hxx"
#include "Settings.hxx"
#include "FSNode.hxx"
#include "OSystem.hxx"
#include "Atari.h"

#if defined(UNIX)
  #include "SettingsUNIX.hxx"
  #include "OSystemUNIX.hxx"
#elif defined(WIN32)
  #include "SettingsWin32.hxx"
  #include "OSystemWin32.hxx"
#elif defined(MAC_OSX)
  #include "SettingsMACOSX.hxx"
  #include "OSystemMACOSX.hxx"

  extern "C" {
  int atariMain(int argc, char* argv[]);
  }
#elif defined(GP2X)
  #include "SettingsGP2X.hxx"
  #include "OSystemGP2X.hxx"
#elif defined(PSP)
  #include "SettingsPSP.hxx"
  #include "OSystemPSP.hxx"
  extern "C" {
    int SDL_main(int argc, char* argv[]);
  }
#else
  #error Unsupported platform!
#endif

#ifdef DEVELOPER_SUPPORT
  #include "Debugger.hxx"
#endif

#ifdef CHEATCODE_SUPPORT
  #include "CheatManager.hxx"
#endif

static void SetupProperties(PropertiesSet& set);
static void Cleanup();

// Pointer to the main parent osystem object or the null pointer
OSystem* theOSystem = (OSystem*) NULL;

extern "C" {

int
main_atari_get_cheat_size()
{
  return 0x10000;
}

int
main_atari_cheat_poke(int addr, unsigned char value)
{
  return theOSystem->console().system().cheat_poke( addr, value );
}

int
main_atari_cheat_peek(int addr, uInt8* p_result)
{
  *p_result = 0;
  return theOSystem->console().system().cheat_peek( addr, *p_result );
}

void
main_atari_send_key_event(int atari_idx, int key_press)
{
  theOSystem->eventHandler().handleEvent(Event::Type(atari_idx), (Int32)key_press);
}

void
main_atari_send_paddle_event(int atari_idx, int resistance)
{
  theOSystem->eventHandler().handleEvent(Event::Type(atari_idx), (Int32)resistance);
}

void
main_atari_emulator_reset()
{
  theOSystem->console().system().reset();
}

int
main_atari_save_state(char *filename)
{
  string md5 = theOSystem->console().properties().get( Cartridge_MD5);
  Serializer out;
  if(!out.open(string(filename))) {
    return 1;
  }
  int result = theOSystem->console().system().saveState(md5, out);

  return (result != 1);
}

int
main_atari_load_state(char *filename)
{
  string md5 = theOSystem->console().properties().get(Cartridge_MD5);
  Deserializer in;
  if(!in.open(string(filename))) {
    return 1;
  }
  int result = theOSystem->console().system().loadState(md5, in);

  return (result != 1);
}

int
main_atari_load_rom(char *filename)
{
  if (theOSystem->createConsole(filename)) {
    return 1;
  }
  return 0;
}

void
main_atari_force_draw_blit()
{
  uInt16* buffer = (uInt16*)psp_sdl_get_blit_addr(0, 0);
  uInt32 pitch  = psp_sdl_get_blit_pitch();
  theOSystem->frameBuffer().drawMediaSourceBlitSurface(pitch, buffer);
}

} //extern "C"


/**
  Setup the properties set by first checking for a user file,
  then a system-wide file.
*/
void SetupProperties(PropertiesSet& set)
{
  // Several properties files can exist, so we attempt to load from
  // all of them.  If the user has specified a properties file, use
  // that one.  Otherwise, load both the system and user properties
  // files, and have the user file override all entries from the
  // system file.

  ostringstream buf;

  string altpro = theOSystem->settings().getString("pro");
  if(altpro != "")
  {
    buf << "User game properties: \'" << altpro << "\'\n";
    set.load(altpro, false);  // don't save alternate properties
  }
  else
  {
    const string& props = theOSystem->propertiesFile();
    buf << "User game properties: \'" << props << "\'\n";
    set.load(props, true);    // do save these properties
  }

# if 0 //LUDO:
  if(theOSystem->settings().getBool("showinfo"))
    cout << buf.str() << endl;
# endif
}


/**
  Does general Cleanup in case any operation failed (or at end of program).
*/
void Cleanup()
{
  if(theOSystem)
    delete theOSystem;

  if(SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO)
    SDL_Quit();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int 
atariMain(int argc, char* argv[])
{
  // Create the parent OSystem object and settings
  theOSystem = new OSystemPSP();
  SettingsPSP settings(theOSystem);

# if 0 //LUDO:
  theOSystem->settings().loadConfig();

  // Take care of commandline arguments
  if(!theOSystem->settings().loadCommandLine(argc, argv))
  {
    Cleanup();
    return 0;
  }

  // Finally, make sure the settings are valid
  // We do it once here, so the rest of the program can assume valid settings
  theOSystem->settings().validate();
# endif

  // Create the full OSystem after the settings, since settings are
  // probably needed for defaults
  theOSystem->create();

  // Create the event handler for the system
  EventHandler handler(theOSystem);

  // Create a properties set for us to use and set it up
  PropertiesSet propertiesSet(theOSystem);
  SetupProperties(propertiesSet);
  theOSystem->attach(&propertiesSet);

# if 0 //LUDO:
  // Check to see if the 'listroms' argument was given
  // If so, list the roms and immediately exit
  if(theOSystem->settings().getBool("listrominfo"))
  {
    propertiesSet.print();
    Cleanup();
    return 0;
  }
# endif

  // Request that the SDL window be centered, if possible
# if 0 //LUDO:
  if(theOSystem->settings().getBool("center"))
    putenv("SDL_VIDEO_CENTERED=1");
# endif

  // Create the framebuffer(s)
  if(!theOSystem->createFrameBuffer())
  {
    cerr << "ERROR: Couldn't set up display.\n";
    Cleanup();
    return 0;
  }

  // Create the sound object
  theOSystem->createSound();

  // Setup the SDL joysticks (must be done after FrameBuffer is created)
# if 0 //LUDO:
  theOSystem->eventHandler().setupJoysticks();
# endif

#ifdef CHEATCODE_SUPPORT
    // Create internal cheat database for all ROMs
    theOSystem->cheat().loadCheatDatabase();
#endif

  //// Main loop ////
  // First we check if a ROM is specified on the commandline.  If so, and if
  //   the ROM actually exists, use it to create a new console.
  // If not, use the built-in ROM launcher.  In this case, we enter 'launcher'
  //   mode and let the main event loop take care of opening a new console/ROM.
# if 0 //LUDO:
  string romfile = argv[argc - 1];
  if(argc == 1 || !FilesystemNode::fileExists(romfile))
    theOSystem->createLauncher();
  else if(theOSystem->createConsole(romfile))
  {
    if(theOSystem->settings().getBool("holdreset"))
      theOSystem->eventHandler().handleEvent(Event::ConsoleReset, 1);

    if(theOSystem->settings().getBool("holdselect"))
      theOSystem->eventHandler().handleEvent(Event::ConsoleSelect, 1);

    if(theOSystem->settings().getBool("holdbutton0"))
      theOSystem->eventHandler().handleEvent(Event::JoystickZeroFire, 1);

#ifdef DEVELOPER_SUPPORT
    Debugger& dbg = theOSystem->debugger();

    // Set up any breakpoint that was on the command line
    // (and remove the key from the settings, so they won't get set again)
    string initBreak = theOSystem->settings().getString("break");
    if(initBreak != "")
    {
      int bp = dbg.stringToValue(initBreak);
      dbg.setBreakPoint(bp, true);
      theOSystem->settings().setString("break", "");
    }

    if(theOSystem->settings().getBool("debug"))
      handler.enterDebugMode();
#endif
  }
  else
  {
    Cleanup();
    return 0;
  }
# else
  //LUDO: We load the default rom file ...
  theOSystem->createConsole("./default.bin");
# endif

# if 0 //LUDO:
  // Swallow any spurious events in the queue
  // These are normally caused by joystick/mouse jitter
  SDL_Event event;
  while(SDL_PollEvent(&event)) /* swallow event */ ;
# endif

  // Start the main loop, and don't exit until the user issues a QUIT command
  theOSystem->mainLoop();

  // Cleanup time ...
  Cleanup();
  return 0;
}
