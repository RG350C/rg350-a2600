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
// $Id: EventHandler.hxx,v 1.85 2006/04/05 12:28:37 stephena Exp $
//============================================================================

#ifndef EVENTHANDLER_HXX
#define EVENTHANDLER_HXX

#include <SDL/SDL.h>

#include "bspf.hxx"
#include "Event.hxx"
#include "Array.hxx"
#include "Control.hxx"
#include "StringList.hxx"
#include "Serializer.hxx"

class Console;
class OSystem;
class DialogContainer;
class EventMappingWidget;
class EventStreamer;

// A wrapper around SDL hat events, so we don't drag SDL
// through all the child classes
enum JoyHat {
  kJHatUp,
  kJHatDown,
  kJHatLeft,
  kJHatRight,
  kJHatCentered
};

enum MouseButton {
  EVENT_LBUTTONDOWN,
  EVENT_LBUTTONUP,
  EVENT_RBUTTONDOWN,
  EVENT_RBUTTONUP,
  EVENT_WHEELDOWN,
  EVENT_WHEELUP
};

// Structure used for action menu items
struct ActionList {
  Event::Type event;
  const char* action;
  char* key;
};

enum {
  kActionListSize = 81
};

// Joystick related items
enum {
  kNumJoysticks  = 8,
  kNumJoyButtons = 24,
  kNumJoyAxis    = 16,
  kNumJoyHats    = 16
};

enum JoyType {
  JT_NONE,
  JT_REGULAR,
  JT_STELLADAPTOR_LEFT,
  JT_STELLADAPTOR_RIGHT
};

struct Stella_Joystick {
  SDL_Joystick* stick;
  JoyType       type;
  string        name;
};

// Used for joystick to mouse emulation
struct JoyMouse {	
  bool active;
  int x, y, x_vel, y_vel, x_max, y_max, x_amt, y_amt, amt,
      x_down_count, y_down_count;
  unsigned int last_time, delay_time, x_down_time, y_down_time;
  int joy_val, old_joy_val;
};

/**
  This class takes care of event remapping and dispatching for the
  Stella core, as well as keeping track of the current 'mode'.

  The frontend will send translated events here, and the handler will
  check to see what the current 'mode' is.

  If in emulation mode, events received from the frontend are remapped and
  sent to the emulation core.  If in menu mode, the events are sent
  unchanged to the menu class, where (among other things) changing key
  mapping can take place.

  @author  Stephen Anthony
  @version $Id: EventHandler.hxx,v 1.85 2006/04/05 12:28:37 stephena Exp $
*/
class EventHandler
{
  friend class EventMappingWidget;

  public:
    /**
      Create a new event handler object
    */
    EventHandler(OSystem* osystem);
 
    /**
      Destructor
    */
    virtual ~EventHandler();

    // Enumeration representing the different states of operation
    enum State { S_NONE, S_EMULATE, S_LAUNCHER, S_MENU, S_CMDMENU, S_DEBUGGER };

    /**
      Returns the event object associated with this handler class.

      @return The event object
    */
    Event* event() { return myEvent; }

    /**
      Set up any joysticks on the system.  This must be called *after* the
      framebuffer has been created, since SDL requires the video to be
      intialized before joysticks can be probed.
    */
    void setupJoysticks();

    /**
      Maps the given stelladaptors to specified ports on a real 2600

      @param sa1  Port for the first Stelladaptor to emulate (left or right)
      @param sa2  Port for the second Stelladaptor to emulate (left or right)
    */
    void mapStelladaptors(const string& sa1, const string& sa2);

    /**
      Collects and dispatches any pending events.  This method should be
      called regularly (at X times per second, where X is the game framerate).

      @param time  The current time in milliseconds.
    */
    void poll(uInt32 time);

    /**
      Set the default action for a joystick button to the given event

      @param event  The event we are assigning
      @param stick  The joystick number
      @param button The joystick button
    */
    void setDefaultJoyMapping(Event::Type event, int stick, int button);

    /**
      Set the default for a joystick axis to the given event

      @param event  The event we are assigning
      @param stick  The joystick number
      @param axis   The joystick axis
      @param value  The value on the given axis
    */
    void setDefaultJoyAxisMapping(Event::Type event, int stick, int axis, int value);

    /**
      Set the default for a joystick hat to the given event

      @param event  The event we are assigning
      @param stick  The joystick number
      @param axis   The joystick axis
      @param value  The value on the given axis
    */
    void setDefaultJoyHatMapping(Event::Type event, int stick, int hat, int value);

    /**
      Returns the current state of the EventHandler

      @return The State type
    */
    inline State state() { return myState; }

    /**
      Returns the current launcher state (decide whether to enter launcher
      on game exit).
    */
    inline bool useLauncher() { return myUseLauncherFlag; }

    /**
      Resets the state machine of the EventHandler to the defaults

      @param state  The current state to set
    */
    void reset(State state);

    /**
      Refresh display according to the current state

      @param forceUpdate  Do a framebuffer update right away, instead
                          of waiting for the next frame
    */
    void refreshDisplay(bool forceUpdate = false);

    /**
      This method pauses the entire emulator.
    */
    void pause(bool status);

    /**
      This method indicates whether a pause event has been received.
    */
    inline bool isPaused() { return myPauseFlag; }

    /**
      This method indicates that the system should terminate.
    */
# if 0 //LUDO:
    void quit() { handleEvent(Event::Quit, 1); }
# else
    void quit() { }
# endif

    /**
      This method indicates whether a quit event has been received.
    */
    inline bool doQuit() { return myQuitFlag; }

    /**
      Save state to explicit state number (debugger uses this)
    */
    void saveState(int state);

    /**
      Load state from explicit state number (debugger uses this)
    */
    void loadState(int state);

    /**
      Sets the mouse to act as paddle 'num'

      @param num          The paddle which the mouse should emulate
      @param showmessage  Print a message to the framebuffer
    */
    void setPaddleMode(int num, bool showmessage = false);

    /**
      Sets the speed of the given paddle

      @param num    The paddle number (0-3)
      @param speed  The speed of paddle movement for the given paddle
    */
    void setPaddleSpeed(int num, int speed);

    /**
      Sets the amount by which paddle jitter is detected.  Quick movements
      of less than this amount constitute jitter, and do not generate
      paddle events.

      @param thresh  The threshold to use for jitter detection
    */
    void setPaddleThreshold(int thresh);

    inline bool kbdAlt(int mod)
    {
  #ifndef MAC_OSX
      return (mod & KMOD_ALT);
  #else
      return ((mod & KMOD_META) && (mod & KMOD_SHIFT));
  #endif
    }

    inline bool kbdControl(int mod)
    {
  #ifndef MAC_OSX
      return (mod & KMOD_CTRL) > 0;
  #else
      return ((mod & KMOD_META) && !(mod & KMOD_SHIFT));
  #endif
    }

    inline bool kbdShift(int mod)
    {
      return (mod & KMOD_SHIFT);
    }

    void enterMenuMode(State state);
    void leaveMenuMode();
    bool enterDebugMode();
    void leaveDebugMode();
    void saveProperties();

    /**
      Send an event directly to the event handler.
      These events cannot be remapped.

      @param type  The event
      @param value The value for the event
    */
    void handleEvent(Event::Type type, Int32 value);

    inline bool frying() { return myFryingFlag; }

    /**
      Create a synthetic SDL mouse motion event based on the given x,y values.

      @param x  The x coordinate of motion, scaled in value
      @param y  The y coordinate of motion, scaled in value
    */
    void createMouseMotionEvent(int x, int y);

    /**
      Create a synthetic SDL mouse button event based on the given x,y values.

      @param x     The x coordinate of motion, scaled in value
      @param y     The y coordinate of motion, scaled in value
      @param state The state of the button click (on or off)
    */
    void createMouseButtonEvent(int x, int y, int state);

    inline SDL_Joystick* getJoystick(int i) { return ourJoysticks[i].stick; }

  private:
    /**
      Bind a key to an event/action and regenerate the mapping array(s)

      @param event  The event we are remapping
      @param key    The key to bind to this event
    */
    bool addKeyMapping(Event::Type event, int key);

    /**
      Bind a joystick button to an event/action and regenerate the
      mapping array(s)

      @param event  The event we are remapping
      @param stick  The joystick number
      @param button The joystick button
    */
    bool addJoyMapping(Event::Type event, int stick, int button);

    /**
      Bind a joystick axis direction to an event/action and regenerate
      the mapping array(s)

      @param event  The event we are remapping
      @param stick  The joystick number
      @param axis   The joystick axis
      @param value  The value on the given axis
    */
    bool addJoyAxisMapping(Event::Type event, int stick, int axis, int value);

    /**
      Bind a joystick hat direction to an event/action and regenerate
      the mapping array(s)

      @param event  The event we are remapping
      @param stick  The joystick number
      @param axis   The joystick hat
      @param value  The value on the given hat
    */
    bool addJoyHatMapping(Event::Type event, int stick, int hat, int value);

    /**
      Erase the specified mapping

      @event  The event for which we erase all mappings
    */
    void eraseMapping(Event::Type event);

    /**
      Resets the event mappings to default values
    */
    void setDefaultMapping();

    /**
      Send a mouse motion event to the handler.

      @param event The mouse motion event generated by SDL
    */
    void handleMouseMotionEvent(SDL_Event& event);

    /**
      Send a mouse button event to the handler.

      @param event The mouse button event generated by SDL
    */
    void handleMouseButtonEvent(SDL_Event& event, int state);

    /**
      Send a joystick button event to the handler

      @param stick  The joystick number
      @param button The joystick button
      @param state  The state of the button (pressed or released)
    */
    void handleJoyEvent(int stick, int button, int state);

    /**
      Send a joystick axis event to the handler

      @param stick  The joystick number
      @param axis   The joystick axis
      @param value  The value on the given axis
    */
    void handleJoyAxisEvent(int stick, int axis, int value);

    /**
      Send a joystick hat event to the handler

      @param stick  The joystick number
      @param axis   The joystick hat
      @param value  The value on the given hat
    */
    void handleJoyHatEvent(int stick, int hat, int value);

    /**
      Detects and changes the eventhandler state

      @param type  The event
      @return      True if the state changed, else false
    */
    inline bool eventStateChange(Event::Type type);

    /**
      The following methods take care of assigning action mappings.
    */
    void setActionMappings();
    void setSDLMappings();
    void setKeymap();
    void setJoymap();
    void setJoyAxisMap();
    void setJoyHatMap();
    void setDefaultKeymap();
    void setDefaultJoymap();
    void setDefaultJoyAxisMap();
    void setDefaultJoyHatMap();
    void saveKeyMapping();
    void saveJoyMapping();
    void saveJoyAxisMapping();
    void saveJoyHatMapping();

    /**
      Tests if a mapping list is valid, both by length and by event count.

      @param list    The string containing the mappings, separated by ':'
      @param map     The result of parsing the string for int mappings
      @param length  The number of items that should be in the list

      @return      True if valid list, else false
    */
    bool isValidList(string& list, IntArray& map, uInt32 length);

    /**
      Tests if a given event should use continuous/analog values.

      @param event  The event to test for analog processing
      @return       True if analog, else false
    */
    inline bool eventIsAnalog(Event::Type event);

    /**
      Tests if the given paddle value is displaying a rapid left/right
      motion, which is also known as jitter.

      @param paddle The paddle to test
      @param value  The value assigned to the paddle
      @return       True if jittering, else false
    */
    inline bool isJitter(int paddle, int value);

    void saveState();
    void changeState();
    void loadState();
    void takeSnapshot();
    void setEventState(State state);

  private:
    // Global OSystem object
    OSystem* myOSystem;

    // Global Event object
    Event* myEvent;

    // The EventStreamer to use for loading/saving eventstreams
    EventStreamer* myEventStreamer;

    // Indicates current overlay object
    DialogContainer* myOverlay;

    // Array of key events, indexed by SDLKey
    Event::Type myKeyTable[SDLK_LAST];

    // Array of joystick button events
    Event::Type myJoyTable[kNumJoysticks][kNumJoyButtons];

    // Array of joystick axis events
    Event::Type myJoyAxisTable[kNumJoysticks][kNumJoyAxis][2];

    // Array of joystick hat events (we don't record diagonals)
    Event::Type myJoyHatTable[kNumJoysticks][kNumJoyHats][4];

    // Array of messages for each Event
    string ourMessageTable[Event::LastType];

    // Array of strings which correspond to the given SDL key
    string ourSDLMapping[SDLK_LAST];

    // Array of joysticks available to Stella
    Stella_Joystick ourJoysticks[kNumJoysticks];

    // Indicates the current state of the system (ie, which mode is current)
    State myState;

    // Indicates the current state to use for state loading/saving
    uInt32 myLSState;

    // Indicates the current pause status
    bool myPauseFlag;

    // Indicates whether to quit the emulator
    bool myQuitFlag;

    // Indicates whether the mouse cursor is grabbed
    bool myGrabMouseFlag;

    // Indicates whether to use launcher mode when exiting a game
    bool myUseLauncherFlag;

    // Indicates whether the joystick emulates the mouse in GUI mode
    bool myEmulateMouseFlag;

    // Indicates whether or not we're in frying mode
    bool myFryingFlag;

    // Indicates which paddle the mouse currently emulates
    Int8 myPaddleMode;

    // Indicates the amount by which we consider a paddle to be jittering
    int myPaddleThreshold;

    // Used for paddle emulation by keyboard or joystick
    JoyMouse myPaddle[4];

    // Type of device on each controller port (based on ROM properties)
    Controller::Type myController[2];

    // Holds static strings for the remap menu
    static ActionList ourActionList[kActionListSize];

    // Lookup table for paddle resistance events
    static const Event::Type Paddle_Resistance[4];

    // Lookup table for paddle button events
    static const Event::Type Paddle_Button[4];

    // Static lookup tables for Stelladaptor axis/button support
    static const Event::Type SA_Axis[2][2][3];
    static const Event::Type SA_Button[2][2][3];
    static const Event::Type SA_DrivingValue[2];
};

#endif
