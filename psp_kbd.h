/*
 *  Copyright (C) 2006 Ludovic Jacomme (ludovic.jacomme@gmail.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

# ifndef _KBD_H_
# define _KBD_H_
# ifdef __cplusplus
extern "C" {
# endif

# define PSP_ALL_BUTTON_MASK 0xFFFF
# define GP2X_ALL_BUTTON_MASK 0xFFFF

 enum atari_keys_emum {
      NoType,
      ConsoleOn, 
      ConsoleOff, 
      ConsoleColor, 
      ConsoleBlackWhite, 
      ConsoleLeftDifficultyA, 
      ConsoleLeftDifficultyB, 
      ConsoleRightDifficultyA, 
      ConsoleRightDifficultyB, 
      ConsoleSelect, 
      ConsoleReset, 
      JoystickZeroUp, 
      JoystickZeroDown, 
      JoystickZeroLeft, 
      JoystickZeroRight, 
      JoystickZeroFire, 
      JoystickOneUp, 
      JoystickOneDown, 
      JoystickOneLeft, 
      JoystickOneRight, 
      JoystickOneFire, 
      PaddleZeroResistance, 
      PaddleZeroFire, 
      PaddleOneResistance, 
      PaddleOneFire, 
      PaddleTwoResistance, 
      PaddleTwoFire, 
      PaddleThreeResistance, 
      PaddleThreeFire, 

    ATARIC_FPS,
    ATARIC_JOY,
    ATARIC_RENDER,
    ATARIC_LOAD,
    ATARIC_SAVE,
    ATARIC_RESET,
    ATARIC_AUTOFIRE,
    ATARIC_INCFIRE,
    ATARIC_DECFIRE,
    ATARIC_SCREEN,

# if 0
      BoosterGripZeroTrigger, 
      BoosterGripZeroBooster, 
      BoosterGripOneTrigger, 
      BoosterGripOneBooster, 
      KeyboardZero1, 
      KeyboardZero2, 
      KeyboardZero3, 
      KeyboardZero4, 
      KeyboardZero5, 
      KeyboardZero6, 
      KeyboardZero7, 
      KeyboardZero8, 
      KeyboardZero9, 
      KeyboardZeroStar, 
      KeyboardZero0, 
      KeyboardZeroPound, 
      KeyboardOne1, 
      KeyboardOne2, 
      KeyboardOne3, 
      KeyboardOne4, 
      KeyboardOne5, 
      KeyboardOne6, 
      KeyboardOne7, 
      KeyboardOne8, 
      KeyboardOne9, 
      KeyboardOneStar, 
      KeyboardOne0, 
      KeyboardOnePound, 
      DrivingZeroClockwise, 
      DrivingZeroCounterClockwise, 
      DrivingZeroValue, 
	    DrivingZeroFire, 
      DrivingOneClockwise, 
      DrivingOneCounterClockwise, 
      DrivingOneValue, 
	    DrivingOneFire, 
# endif
      ATARI_MAX_KEY
  };

# define KBD_UP           0
# define KBD_RIGHT        1
# define KBD_DOWN         2
# define KBD_LEFT         3
# define KBD_TRIANGLE     4
# define KBD_CIRCLE       5
# define KBD_CROSS        6
# define KBD_SQUARE       7
# define KBD_SELECT       8
# define KBD_START        9
# define KBD_LTRIGGER    10
# define KBD_RTRIGGER    11
# define KBD_FIRE        12

# define KBD_MAX_BUTTONS 13

# define KBD_JOY_UP      13
# define KBD_JOY_RIGHT   14
# define KBD_JOY_DOWN    15
# define KBD_JOY_LEFT    16

# define KBD_ALL_BUTTONS 17

# define KBD_UNASSIGNED         -1

# define KBD_LTRIGGER_MAPPING   -2
# define KBD_RTRIGGER_MAPPING   -3
# define KBD_NORMAL_MAPPING     -1

 struct atari_key_trans {
   enum atari_keys_emum  key;
   char name[32];
 };
  

  extern int psp_screenshot_mode;
  extern int psp_kbd_mapping[ KBD_ALL_BUTTONS ];
  extern int psp_kbd_mapping_L[ KBD_ALL_BUTTONS ];
  extern int psp_kbd_mapping_R[ KBD_ALL_BUTTONS ];
  extern int psp_kbd_presses[ KBD_ALL_BUTTONS ];
  extern int kbd_ltrigger_mapping_active;
  extern int kbd_rtrigger_mapping_active;

  extern struct atari_key_trans psp_atari_key_to_name[ATARI_MAX_KEY];

  extern void psp_kbd_default_settings();
  extern int  psp_update_keys(void);
  extern void kbd_wait_start(void);
  extern void psp_init_keyboard(void);
  extern void psp_kbd_wait_no_button(void);
  extern int  psp_kbd_is_danzeff_mode(void);
  extern int psp_kbd_load_mapping(char *kbd_filename);
  extern int psp_kbd_save_mapping(char *kbd_filename);
  extern void psp_kbd_display_active_mapping(void);
  extern void kbd_change_auto_fire(int auto_fire);

# ifdef __cplusplus
}
# endif
# endif
