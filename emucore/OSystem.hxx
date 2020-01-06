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
// $Id: OSystem.hxx,v 1.42 2006/03/27 12:52:19 stephena Exp $
//============================================================================

#ifndef OSYSTEM_HXX
#define OSYSTEM_HXX

class PropertiesSet;

class Menu;
class CommandMenu;
class Launcher;
class Debugger;
class CheatManager;

#include "EventHandler.hxx"
#include "FrameBuffer.hxx"
#include "Sound.hxx"
#include "Settings.hxx"
#include "Console.hxx"
#include "StringList.hxx"
# if 0 //LUDO:
#include "Font.hxx"
# endif

#include "bspf.hxx"


/**
  This class provides an interface for accessing operating system specific
  functions.  It also comprises an overall parent object, to which all the
  other objects belong.

  @author  Stephen Anthony
  @version $Id: OSystem.hxx,v 1.42 2006/03/27 12:52:19 stephena Exp $
*/
class OSystem
{
  public:
    /**
      Create a new OSystem abstract class
    */
    OSystem();

    /**
      Destructor
    */
    virtual ~OSystem();

    /**
      Create all child objects which belong to this OSystem
    */
    virtual bool create();

  public:
    /**
      Adds the specified eventhandler to the system.

      @param eventhandler The eventhandler to add 
    */
    void attach(EventHandler* eventhandler) { myEventHandler = eventhandler; }

    /**
      Adds the specified settings object to the system.

      @param settings The settings object to add 
    */
    void attach(Settings* settings) { mySettings = settings; }

    /**
      Adds the specified game properties set to the system.

      @param propset The properties set to add 
    */
    void attach(PropertiesSet* propset) { myPropSet = propset; }

    /**
      Get the event handler of the system

      @return The event handler
    */
    inline EventHandler& eventHandler() const { return *myEventHandler; }

    /**
      Get the frame buffer of the system

      @return The frame buffer
    */
    inline FrameBuffer& frameBuffer() const { return *myFrameBuffer; }

    /**
      Get the sound object of the system

      @return The sound object
    */
    inline Sound& sound() const { return *mySound; }

    /**
      Get the settings object of the system

      @return The settings object
    */
    inline Settings& settings() const { return *mySettings; }

    /**
      Get the set of game properties for the system

      @return The properties set object
    */
    inline PropertiesSet& propSet() const { return *myPropSet; }

    /**
      Get the console of the system.

      @return The console object
    */
    inline Console& console(void) const { return *myConsole; }

    /**
      Get the settings menu of the system.

      @return The settings menu object
    */
    inline Menu& menu(void) const { return *myMenu; }

    /**
      Get the command menu of the system.

      @return The command menu object
    */
    inline CommandMenu& commandMenu(void) const { return *myCommandMenu; }

    /**
      Get the ROM launcher of the system.

      @return The launcher object
    */
    inline Launcher& launcher(void) const { return *myLauncher; }

#ifdef DEVELOPER_SUPPORT
    /**
      Get the ROM debugger of the system.

      @return The debugger object
    */
    inline Debugger& debugger(void) const { return *myDebugger; }
#endif

#ifdef CHEATCODE_SUPPORT
    /**
      Get the cheat manager of the system.

      @return The cheatmanager object
    */
    inline CheatManager& cheat(void) const { return *myCheatManager; }
#endif

    /**
      Get the font object of the system

      @return The font reference
    */
# if 0 //LUDO:
    inline const GUI::Font& font() const { return *myFont; }
# endif

    /**
      Get the launcher font object of the system

      @return The font reference
    */
# if 0 //LUDO:
    inline const GUI::Font& launcherFont() const { return *myLauncherFont; }
# endif

    /**
      Get the console font object of the system

      @return The console font reference
    */
# if 0 //LUDO:
    inline const GUI::Font& consoleFont() const { return *myConsoleFont; }
# endif

    /**
      Set the framerate for the video system.  It's placed in this class since
      the mainLoop() method is defined here.

      @param framerate  The video framerate to use
    */
    virtual void setFramerate(uInt32 framerate);

    /**
      Get the current framerate for the video system.

      @return  The video framerate currently in use
    */
    uInt32 frameRate() const { return myDisplayFrameRate; }

    /**
      Return the default directory for storing data.
    */
    const string& baseDir() const { return myBaseDir; }

    /**
      Return the directory for storing state files.
    */
    const string& stateDir() const { return myStateDir; }

    /**
      This method should be called to get the full path of the config file.

      @return String representing the full path of the config filename.
    */
    const string& configFile() const { return myConfigFile; }

    /**
      This method should be called to get the full path of the
      properties file (stella.pro).

      @return String representing the full path of the properties filename.
    */
    const string& propertiesFile() const { return myPropertiesFile; }

    /**
      This method should be called to get the full path of the gamelist
      cache file (used by the Launcher to show a listing of available games).

      @return String representing the full path of the gamelist cache file.
    */
    const string& cacheFile() const { return myGameListCacheFile; }

    /**
      This method should be called to get the full path of the currently
      loaded ROM.

      @return String representing the full path of the ROM file.
    */
    const string& romFile() const { return myRomFile; }

    /**
      Creates the various framebuffers/renderers available in this system
      (for now, that means either 'software' or 'opengl').

      @return Success or failure of the framebuffer creation
    */
    bool createFrameBuffer(bool showmessage = false);

    /**
      Switches between software and OpenGL framebuffer modes.
    */
    void toggleFrameBuffer();

    /**
      Creates the various sound devices available in this system
      (for now, that means either 'SDL' or 'Null').
    */
    void createSound();

    /**
      Creates a new game console from the specified romfile.

      @param romfile  The full pathname of the ROM to use
      @return  True on successful creation, otherwise false
    */
    bool createConsole(const string& romfile = "");

    /**
      Creates a new ROM launcher, to select a new ROM to emulate.
    */
    void createLauncher();

    /**
      The features which are conditionally compiled into Stella.

      @return  The supported features
    */
    const string& features() const { return myFeatures; }

    /**
      Open the given ROM and return an array containing its contents.

      @param rom    The absolute pathname of the ROM file
      @param md5    The md5 calculated from the ROM file
      @param image  A pointer to store the ROM data
                    Note, the calling method is responsible for deleting this
      @param size   The amount of data read into the image array
      @return  False on any errors, else true
    */
    bool openROM(const string& rom, string& md5, uInt8** image, int* size);

  public:
    //////////////////////////////////////////////////////////////////////
    // The following methods are system-specific and must be implemented
    // in derived classes.
    //////////////////////////////////////////////////////////////////////
    /**
      This method runs the main loop.  Since different platforms
      may use different timing methods and/or algorithms, this method has
      been abstracted to each platform.
    */
    virtual void mainLoop() = 0;

    /**
      This method returns number of ticks in microseconds.

      @return Current time in microseconds.
    */
    virtual uInt32 getTicks() = 0;

    /**
      This method queries the dimensions of the screen for the given device.
    */
    virtual void getScreenDimensions(int& width, int& height) = 0;

    //////////////////////////////////////////////////////////////////////
    // The following methods are system-specific and can be overrided in
    // derived classes.  Otherwise, the base methods will be used.
    //////////////////////////////////////////////////////////////////////
    /**
      This method determines the default mapping of joystick buttons to
      Stella events for a specific system/platform.
    */
    virtual void setDefaultJoymap();

    /**
      This method determines the default mapping of joystick axis to
      Stella events for a specific system/platform.
    */
    virtual void setDefaultJoyAxisMap();

    /**
      This method determines the default mapping of joystick hats to
      Stella events for a specific system/platform.
    */
    virtual void setDefaultJoyHatMap();

    /**
      This method creates events from platform-specific hardware.
    */
    virtual void pollEvent();

    /**
      This method answers whether the given button as already been
      handled by the pollEvent() method, and as such should be ignored
      in the main event handler.
    */
    virtual bool joyButtonHandled(int button);

    /**
      Informs the OSystem of a change in EventHandler state.
    */
    virtual void stateChanged(EventHandler::State state);

    /**
      Informs the OSystem of a change in pause status.
    */
    virtual void pauseChanged(bool status);

  protected:
    /**
      Set the base directory for all Stella files
    */
    void setBaseDir(const string& basedir);

    /**
      Set the directory where state files are stored
    */
    void setStateDir(const string& statedir);

    /**
      Set the locations of game properties file
    */
    void setPropertiesDir(const string& path);

    /**
      Set the locations of config file
    */
    void setConfigFile(const string& file);

    /**
      Set the location of the gamelist cache file
    */
    void setCacheFile(const string& cachefile) { myGameListCacheFile = cachefile; }

  protected:
    // Pointer to the EventHandler object
    EventHandler* myEventHandler;

    // Pointer to the FrameBuffer object
    FrameBuffer* myFrameBuffer;

    // Pointer to the Sound object
    Sound* mySound;

    // Pointer to the Settings object
    Settings* mySettings;

    // Pointer to the PropertiesSet object
    PropertiesSet* myPropSet;

    // Pointer to the (currently defined) Console object
    Console* myConsole;

    // Pointer to the Menu object
    Menu* myMenu;

    // Pointer to the CommandMenu object
    CommandMenu* myCommandMenu;

    // Pointer to the Launcher object
    Launcher* myLauncher;

    // Pointer to the Debugger object
    Debugger* myDebugger;

    // Pointer to the CheatManager object
    CheatManager* myCheatManager;

    // Number of times per second to iterate through the main loop
    uInt32 myDisplayFrameRate;

    // Time per frame for a video update, based on the current framerate
    uInt32 myTimePerFrame;

  private:
    string myBaseDir;
    string myStateDir;

    string myConfigFile;
    string myPropertiesFile;

    string myGameListCacheFile;
    string myRomFile;

    string myFeatures;

# if 0 //LUDO:
    // The font object to use for the normal in-game GUI
    GUI::Font* myFont;

    // The font object to use for the ROM launcher
    GUI::Font* myLauncherFont;

    // The font object to use for the console/debugger 
    GUI::Font* myConsoleFont;
# endif

  private:
    // Copy constructor isn't supported by this class so make it private
    OSystem(const OSystem&);

    // Assignment operator isn't supported by this class so make it private
    OSystem& operator = (const OSystem&);
};

#endif
