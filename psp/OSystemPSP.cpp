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
// $Id: OSystemPSP.cxx,v 1.5 2006/01/08 02:28:04 stephena Exp $
//============================================================================

#include <cstdlib>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "bspf.hxx"
#include "OSystem.hxx"
#include "OSystemPSP.hxx"
//LUDO:
#include "psp_kbd.h"
#include "psp_sdl.h"

#ifdef HAVE_GETTIMEOFDAY
  #include <time.h>
  #include <sys/time.h>
#endif


/**
  Each derived class is responsible for calling the following methods
  in its constructor:

  setBaseDir()
  setStateDir()
  setPropertiesFiles()
  setConfigFiles()
  setCacheFile()

  And for initializing the following variables:

  myDriverList (a StringList)

  See OSystem.hxx for a further explanation
*/

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystemPSP::OSystemPSP()
{
  char buffer[128];
  getcwd(buffer, 128);
  // First set variables that the OSystem needs
  string basedir = string(buffer);
  setBaseDir(basedir);

  string statedir = basedir + "/state";
  setStateDir(statedir);

# if 0 //LUDO:
  string userPropertiesFile   = basedir + "/stella.pro";
  string systemPropertiesFile = "/etc/stella.pro";
  setConfigFiles(userPropertiesFile, systemPropertiesFile);

  string userConfigFile   = basedir + "/stellarc";
  string systemConfigFile = "/etc/stellarc";
  setConfigFiles(userConfigFile, systemConfigFile);

  string cacheFile = basedir + "/stella.cache";
  setCacheFile(cacheFile);
# endif

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystemPSP::~OSystemPSP()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystemPSP::mainLoop()
{
  // These variables are common to both timing options
  // and are needed to calculate the overall frames per second.
# if 0 //LUDO:
  uInt32 frameTime = 0, numberOfFrames = 0;

  // Set up less accurate timing stuff
  uInt32 startTime, virtualTime, currentTime;

  // Set the base for the timers
  virtualTime = getTicks();
  frameTime = 0;
# endif

# if 0 //LUDO:
  // Overclock CPU to 333MHz
  if (settings().getBool("pspoverclock"))
  {
    scePowerSetClockFrequency(333,333,166);
    fprintf(stderr,"OSystemPSP::mainLoop overclock to 333\n");
  }
  else
  {
    fprintf(stderr,"OSystemPSP::mainLoop NOT overclock\n");
  }
# endif
  psp_sdl_black_screen();

  // Main game loop
  for(;;)
  {
    // Exit if the user wants to quit
    if(myEventHandler->doQuit())
      break;

//LUDO:
# if 0
    startTime = getTicks();
    myEventHandler->poll(startTime);
# endif
    myEventHandler->poll(0);
    myFrameBuffer->update();
# if 0
    currentTime = getTicks();
    virtualTime += myTimePerFrame;
    if(currentTime < virtualTime) {
      SDL_Delay((virtualTime - currentTime)/1000);
    }

    currentTime = getTicks() - startTime;
    frameTime += currentTime;
    ++numberOfFrames;
# endif
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 OSystemPSP::getTicks()
{
#if defined(HAVE_GETTIMEOFDAY)
  timeval now;
  gettimeofday(&now, 0);
  return (uInt32) (now.tv_sec * 1000000 + now.tv_usec);
#else
  return (uInt32) SDL_GetTicks() * 1000;
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystemPSP::getScreenDimensions(int& width, int& height)
{
  width  = 480;
  height = 272;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystemPSP::setDefaultJoymap()
{
# if 0 //LUDO:
  myEventHandler->setDefaultJoyMapping(Event::TakeSnapshot, 0, 0);      // Triangle
  myEventHandler->setDefaultJoyMapping(Event::LoadState, 0, 1);         // Circle
  myEventHandler->setDefaultJoyMapping(Event::JoystickZeroFire, 0, 2);  // Cross
  myEventHandler->setDefaultJoyMapping(Event::SaveState, 0, 3);         // Square
  myEventHandler->setDefaultJoyMapping(Event::MenuMode, 0, 4);          // Left trigger
  myEventHandler->setDefaultJoyMapping(Event::CmdMenuMode, 0, 5);       // Right trigger
  myEventHandler->setDefaultJoyMapping(Event::JoystickZeroDown, 0, 6);  // Down
  myEventHandler->setDefaultJoyMapping(Event::JoystickZeroLeft, 0, 7);  // Left
  myEventHandler->setDefaultJoyMapping(Event::JoystickZeroUp, 0, 8);    // Up
  myEventHandler->setDefaultJoyMapping(Event::JoystickZeroRight, 0, 9); // Right
  myEventHandler->setDefaultJoyMapping(Event::ConsoleSelect, 0, 10);    // Select
  myEventHandler->setDefaultJoyMapping(Event::ConsoleReset, 0, 11);     // Start
  myEventHandler->setDefaultJoyMapping(Event::NoType, 0, 12);           // Home
  myEventHandler->setDefaultJoyMapping(Event::NoType, 0, 13);           // Hold
# endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystemPSP::setDefaultJoyAxisMap()
{
}
