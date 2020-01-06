
    Welcome to DINGUX-2600

The Stella team :

   Bradford Mott       Project management and emulation core developer
                       (original author of Stella)

   Stephen Anthony     Author of the SDL port of Stella and emulation core
   Joe D'Andrea        Maintainer of the Solaris build of Stella
   Doodle              Current maintainer of the OS/2 port of Stella
   Mark Grebe          Author/maintainer of the Mac OSX port of Stella
   Erik "Voch" Kovach  Maintainer of the 'stella.pro' game properties file
   Kostas Nakos        Author/maintainer of the WinCE port of Stella
   Darrell Spice Jr.   Original author of the OS/2 port of Stella
   Eckhard Stolberg    Emulation core development
   Aenea               First PSP port version of Stella
   Brian Watson        Emulation core development and debugger support
   Alex Zaballa        Author/maintainer of the DINGUX port of Stella
   Ludovic Jacomme     Another PSP & DINGUX port of Stella

   see http://stella.sourceforge.net/ for further informations
   
Author of another PSP, WIZ, GP2X-F100, Dingoo/Dingux port versions

  Ludovic.Jacomme also known as Zx-81 (see http://zx81.zx81.free.fr)


1. INTRODUCTION
   ------------

  Stella is the best emulator of Atari 2600 game console, 
  running on many different systems, such as Linux, Solaris, 
  Windows, MacOS/X WinCE, OS/2, DINGUX.

  Dingux2600 is a port on Dingux of my previous GP2X-Wiz port version.

  This package is under GPL Copyright, read COPYING file for
  more information about it.


2. INSTALLATION
   ------------

  Unzip the zip file, and copy the content of the directory local to your
  SD memory.

  Put your roms files on "roms" sub-directory. 

  For any comments or questions on this version, please visit 
  http://zx81.zx81.free.fr, http://zx81.dcemu.co.uk or 
  http://www.gp32x.com/


3. CONTROL
   ------------

  DINGUX        Atari 2600
  
  Y          R Diff A
  X          Reset    
  B          Fire
  A          Select
  Up         Up
  Down       Down
  Left       Left
  Right      Right

  LTrigger   Toogle with L keyboard mapping
  RTrigger   Toggle with R keyboard mapping

  LTrigger mapping :

  DINGUX        Atari 2600
    
  Y          Hotkey FPS
  X          Hotkey Load state
  B          Hotkey Save state
  A          Hotkey render
  Up         Up
  Down       Down
  Left       Left
  Right      Right

  RTrigger mapping :

  DINGUX        Atari 2600 
    
  Y          R Diff B
  X          Reset 
  B          Hotkey auto-fire
  A          Select 
  Up         Up
  Down       Down
  Left       Left
  Right      Right


  Press Menu      to enter in emulator main menu.
  Press Select    open/close the virtual keyboard

  X          Go Up directory
  B          Valid
  A          Valid
  Y          Go Back to the emulator window

  The On-Screen Keyboard of "Danzel" and "Jeff Chen"

  Use digital pad to choose one of the 9 squares, and
  use X, Y, A, B to choose one of the 4 letters of the
  highlighted square.

  Use LTrigger and RTrigger to see other 9 squares figures.


4. LOADING ROM FILES (.A26 or .BIN)
   ------------

  If you want to load rom images in the virtual drive of your emulator,
  you have to put your rom file (with .zip, .bin or .a26 file extension)
  on your SD memory in the 'roms' directory. 

  Then, while inside Atari 2600 emulator, just press SELECT to enter in the
  emulator main menu, choose "Load ROM" and then using the file selector
  choose one game file to load in your emulator. Back to the emulator window,
  your game should run automatically.

5. COMMENTS
   ------------

You can write your own comments for games using the "Comment" menu.  The first
line of your comments would then be displayed in the file requester menu while
selecting the given file name (snapshot, disk, keyboard, settings).


6. LOADING KEY MAPPING FILES
   ------------

  For given games, the default keyboard mapping between DINGUX Keys and Atari 2600
  keys, is not suitable, and the game can't be played on DINGUX-2600.

  To overcome the issue, you can write your own mapping file. Using notepad for
  example you can edit a file with the .kbd extension and put it in the kbd 
  directory.

  For the exact syntax of those mapping files, have a look on sample files
  already presents in the kbd directory (default.kbd etc ...).

  After writting such keyboard mapping file, you can load them using 
  the main menu inside the emulator.

  If the keyboard filename is the same as the rom file (.a26) then 
  when you load this rom file, the corresponding keyboard file is 
  automatically loaded !

  You can now use the Keyboard menu and edit, load and save your
  keyboard mapping files inside the emulator. The Save option save the .kbd
  file in the kbd directory using the "Game Name" as filename. The game name
  is displayed on the right corner in the emulator menu.

7. CHEAT CODE (.CHT)
----------

  You can use cheat codes with this emulator.  You can add your own cheat
  codes in the cheat.txt file and then import them in the cheat menu.  

  All cheat codes you have specified for a game can be save in a CHT file in
  'cht' folder.  Those cheat codes would then be automatically loaded when you
  start the game.

  The CHT file format is the following :
  #
  # Enable, Address, Value, Comment
  #
  1,36f,3,Cheat comment
  
    Using the Cheat menu you can search for modified
  bytes in RAM between current time and the last time
  you saved the RAM. It might be very usefull to find
  "poke" address by yourself, monitoring for example
  life numbers.
  
  To find a new "poke address" you can proceed as
  follow :
  
  Let's say you're playing Moon patrol and you want to
  find the memory address where "number lives" is
  stored.
  
  . Start a new game in Moon patrol
  . Enter in the cheat menu. 
  . Choose Save Ram to save initial state of the memory. 
  . Specify the number of lives you want to find in
    "Scan Old Value" field.
    (for Glouton the initial lives number is 3)
  . Go back to the game and loose a life.
  . Enter in the cheat menu. 
  . Specify the number of lives you want to find in
    "Scan New Value" field.
    (for Moon patrol the lives number is now 2)
  . In Add Cheat you have now one matching Address
  . Specify the Poke value you want (for example 3) 
    and add a new cheat with this address / value.
  
  The cheat is now activated in the cheat list and you
  can save it using the "Save cheat" menu.
  
  Let's enjoy Moon patrol with infinite life !!
  
8. SETTINGS
------------

  You can modify several settings value in the settings menu of this emulator.
  The following parameters are available :

  Sound enable : 
    enable or disable the sound
  
  Speed limiter :
    limit the speed to a given fps value
  
  Skip frame : 
    to skip frame and increase emulator speed
  
  Display fps : 
    display real time fps value 
  
  Render mode : 
    many render modes are available with different
    geometry that should covered all games
    requirements
  
  Delta Y : 
    move the center of the screen vertically
  
  Vsync : 
    wait for vertical signal between each frame displayed
  
  Flicker mode : 
    set flicker mode (none, simple, phosphor or average)
    
  Clock frequency : 
    DINGUX clock frequency, by default the value is set
    to 200Mhz, and should be enough for most of all
    games.

9. JOYSTICK SETTINGS
------------

  You can modify several joystick settings value in the settings menu of this
  emulator.  The following parameters are available :

  Swap Analog/Cursor : 
    swap key mapping between DINGUX analog pad and DINGUX
    digital pad
  
  Active Joystick : 
    Joystick player, it could be 1 or 2
  
  Paddle enable :
    enable or disable paddle
  
  Paddle speed :
    specify paddle speed factor (from 1 to 3)
  
  Auto fire period : 
    auto fire period
  
  Auto fire mode : 
    auto fire mode active or not
  

10. FLICKERING 
   ------------

  On several games such as Asteroids or Missile Command, the screen flicks,
  or the color are dark. You can then change the Flicker mode parameters in 
  the Settings menu. For example, Asteroids is very nice using the "Simple"
  anti-flicker mode.

11. AUTO-FIRE
------------

It may happens that autofire can not be stopped.  You have then to go to the
settings menu and set auto fire option to off ...
  
12. COMPILATION
   ------------

  It has been developped under Linux FC9 using gcc with DINGUX SDK. 
  All tests have been done using a Dingoo with Dingux installed
  To rebuild the homebrew run the Makefile in the src archive.


  Enjoy,

            Zx
