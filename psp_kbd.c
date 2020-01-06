/*
 *  Copyright (C) 2009 Ludovic Jacomme (ludovic.jacomme@gmail.com)
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#include <SDL/SDL.h>

#include "Atari.h"
#include "global.h"
#include "psp_kbd.h"
#include "psp_menu.h"
#include "psp_sdl.h"
#include "psp_danzeff.h"

# define KBD_MIN_ANALOG_TIME  150000
# define KBD_MIN_START_TIME   800000
# define KBD_MAX_EVENT_TIME   500000
# define KBD_MIN_PENDING_TIME 300000
# define KBD_MIN_HOTKEY_TIME  1000000
# define KBD_MIN_DANZEFF_TIME 150000
# define KBD_MIN_COMMAND_TIME 100000
# define KBD_MIN_BATTCHECK_TIME 90000000 
# define KBD_MIN_AUTOFIRE_TIME   1000000

 static gp2xCtrlData    loc_button_data;
 static unsigned int   loc_last_event_time = 0;
 static unsigned int   loc_last_hotkey_time = 0;
 static long           first_time_stamp = -1;
 static long           first_time_auto_stamp = -1;
 static char           loc_button_press[ KBD_MAX_BUTTONS ]; 
 static char           loc_button_release[ KBD_MAX_BUTTONS ]; 
 static unsigned int   loc_button_mask[ KBD_MAX_BUTTONS ] =
 {
   GP2X_CTRL_UP         , /*  KBD_UP         */
   GP2X_CTRL_RIGHT      , /*  KBD_RIGHT      */
   GP2X_CTRL_DOWN       , /*  KBD_DOWN       */
   GP2X_CTRL_LEFT       , /*  KBD_LEFT       */
   GP2X_CTRL_TRIANGLE   , /*  KBD_TRIANGLE   */
   GP2X_CTRL_CIRCLE     , /*  KBD_CIRCLE     */
   GP2X_CTRL_CROSS      , /*  KBD_CROSS      */
   GP2X_CTRL_SQUARE     , /*  KBD_SQUARE     */
   GP2X_CTRL_SELECT     , /*  KBD_SELECT     */
   GP2X_CTRL_START      , /*  KBD_START      */
   GP2X_CTRL_LTRIGGER   , /*  KBD_LTRIGGER   */
   GP2X_CTRL_RTRIGGER   , /*  KBD_RTRIGGER   */
   GP2X_CTRL_FIRE,        /*  KBD_FIRE       */
 };

 static char loc_button_name[ KBD_ALL_BUTTONS ][20] =
 {
   "UP",
   "RIGHT",
   "DOWN",
   "LEFT",
# if defined(DINGUX_MODE) 
   "X",      // Triangle
   "A",      // Circle
   "B",      // Cross
   "Y",      // Square
# else
   "Y",      // Triangle
   "B",      // Circle
   "X",      // Cross
   "A",      // Square
# endif
   "SELECT",
   "START",
   "LTRIGGER",
   "RTRIGGER",
   "JOY_FIRE",
   "JOY_UP",
   "JOY_RIGHT",
   "JOY_DOWN",
   "JOY_LEFT"
 };

 static char loc_button_name_L[ KBD_ALL_BUTTONS ][20] =
 {
   "L_UP",
   "L_RIGHT",
   "L_DOWN",
   "L_LEFT",
# if defined(DINGUX_MODE) 
   "L_X",      // Triangle
   "L_A",      // Circle
   "L_B",      // Cross
   "L_Y",      // Square
# else
   "L_Y",      // Triangle
   "L_B",      // Circle
   "L_X",      // Cross
   "L_A",      // Square
# endif
   "L_SELECT",
   "L_START",
   "L_LTRIGGER",
   "L_RTRIGGER",
   "L_JOY_FIRE",
   "L_JOY_UP",
   "L_JOY_RIGHT",
   "L_JOY_DOWN",
   "L_JOY_LEFT"
 };
 
  static char loc_button_name_R[ KBD_ALL_BUTTONS ][20] =
 {
   "R_UP",
   "R_RIGHT",
   "R_DOWN",
   "R_LEFT",
# if defined(DINGUX_MODE)
   "R_X",      // Triangle
   "R_A",      // Circle
   "R_B",      // Cross
   "R_Y",      // Square
# else
   "R_Y",      // Triangle
   "R_B",      // Circle
   "R_X",      // Cross
   "R_A",      // Square
# endif
   "R_SELECT",
   "R_START",
   "R_LTRIGGER",
   "R_RTRIGGER",
   "R_JOY_FIRE",
   "R_JOY_UP",
   "R_JOY_RIGHT",
   "R_JOY_DOWN",
   "R_JOY_LEFT"
 };
 
  struct atari_key_trans psp_atari_key_to_name[ATARI_MAX_KEY]=
  {
    { NoType,                      "None" },
    { ConsoleOn,                   "On" }, 
    { ConsoleOff,                  "Off" }, 
    { ConsoleColor,                "Color" }, 
    { ConsoleBlackWhite,           "B&W" }, 
    { ConsoleLeftDifficultyA,      "LDiffA" }, 
    { ConsoleLeftDifficultyB,      "LDiffB" }, 
    { ConsoleRightDifficultyA,     "RDiffA" }, 
    { ConsoleRightDifficultyB,     "RDiffB" }, 
    { ConsoleSelect,               "Select" }, 
    { ConsoleReset,                "Reset" }, 
    { JoystickZeroUp,              "J0_Up" }, 
    { JoystickZeroDown,            "J0_Down" }, 
    { JoystickZeroLeft,            "J0_Left" }, 
    { JoystickZeroRight,           "J0_Right" }, 
    { JoystickZeroFire,            "J0_Fire" }, 
    { JoystickOneUp,               "J1_Up" }, 
    { JoystickOneDown,             "J1_Down" }, 
    { JoystickOneLeft,             "J1_Left" }, 
    { JoystickOneRight,            "J1_Right" }, 
    { JoystickOneFire,             "J1_Fire" },
    { PaddleZeroResistance,        "P0_Resistance" }, 
    { PaddleZeroFire,              "P0_Fire" }, 
    { PaddleOneResistance,         "P1_Resistance" }, 
    { PaddleOneFire,               "P1_Fire" }, 
    { PaddleTwoResistance,         "P2_Resistance" }, 
    { PaddleTwoFire,               "P2_Fire" }, 
    { PaddleThreeResistance,       "P3_Resistance" }, 
    { PaddleThreeFire,             "P3_Fire" } ,
    { ATARIC_FPS,      "C_FPS" },
    { ATARIC_JOY,      "C_JOY" },
    { ATARIC_RENDER,   "C_RENDER" },
    { ATARIC_LOAD,     "C_LOAD" },
    { ATARIC_SAVE,     "C_SAVE" },
    { ATARIC_RESET,    "C_RESET" },
    { ATARIC_AUTOFIRE, "C_AUTOFIRE" },
    { ATARIC_INCFIRE,  "C_INCFIRE" },
    { ATARIC_DECFIRE,  "C_DECFIRE" },
    { ATARIC_SCREEN,   "C_SCREEN" }
  };

 static int loc_default_mapping[ KBD_ALL_BUTTONS ] = {
   JoystickOneUp        , /*  KBD_UP         */
   JoystickOneRight     , /*  KBD_RIGHT      */
   JoystickOneDown      , /*  KBD_DOWN       */
   JoystickOneLeft      , /*  KBD_LEFT       */
   ConsoleReset         , /*  KBD_TRIANGLE   */
   ConsoleSelect        , /*  KBD_CIRCLE     */
   JoystickZeroFire      , /*  KBD_CROSS      */
   ConsoleRightDifficultyA,  /*  KBD_SQUARE     */
   -1                    , /*  KBD_SELECT     */
   -1                    , /*  KBD_START      */
   KBD_LTRIGGER_MAPPING  , /*  KBD_LTRIGGER   */
   KBD_RTRIGGER_MAPPING  , /*  KBD_RTRIGGER   */
   JoystickZeroFire      , /*  KBD_JOY_FIRE   */
   JoystickZeroUp        , /*  KBD_JOY_UP     */
   JoystickZeroRight     , /*  KBD_JOY_RIGHT  */
   JoystickZeroDown      , /*  KBD_JOY_DOWN   */
   JoystickZeroLeft        /*  KBD_JOY_LEFT   */
  };

 static int loc_default_mapping_L[ KBD_ALL_BUTTONS ] = {
   JoystickOneUp        , /*  KBD_UP         */
   JoystickOneRight     , /*  KBD_RIGHT      */
   JoystickOneDown      , /*  KBD_DOWN       */
   JoystickOneLeft      , /*  KBD_LEFT       */
   ATARIC_LOAD           , /*  KBD_TRIANGLE   */
   ATARIC_RENDER         , /*  KBD_CIRCLE     */
   ATARIC_SAVE          , /*  KBD_CROSS      */
   ATARIC_FPS             ,  /*  KBD_SQUARE     */
   -1                    , /*  KBD_SELECT     */
   -1                    , /*  KBD_START      */
   KBD_LTRIGGER_MAPPING  , /*  KBD_LTRIGGER   */
   KBD_RTRIGGER_MAPPING  , /*  KBD_RTRIGGER   */
   JoystickZeroFire      , /*  KBD_JOY_FIRE   */
   JoystickZeroUp        , /*  KBD_JOY_UP     */
   JoystickZeroRight     , /*  KBD_JOY_RIGHT  */
   JoystickZeroDown      , /*  KBD_JOY_DOWN   */
   JoystickZeroLeft        /*  KBD_JOY_LEFT   */
  };

 static int loc_default_mapping_R[ KBD_ALL_BUTTONS ] = {
   JoystickOneUp        , /*  KBD_UP         */
   ATARIC_INCFIRE       , /*  KBD_RIGHT      */
   JoystickOneDown      , /*  KBD_DOWN       */
   ATARIC_DECFIRE       , /*  KBD_LEFT       */
   ConsoleReset         , /*  KBD_TRIANGLE   */
   ConsoleSelect        , /*  KBD_CIRCLE     */
   ATARIC_AUTOFIRE      , /*  KBD_CROSS      */
   ConsoleRightDifficultyB,  /*  KBD_SQUARE     */
   -1                    , /*  KBD_SELECT     */
   -1                    , /*  KBD_START      */
   KBD_LTRIGGER_MAPPING  , /*  KBD_LTRIGGER   */
   KBD_RTRIGGER_MAPPING  , /*  KBD_RTRIGGER   */
   JoystickZeroFire      , /*  KBD_JOY_FIRE   */
   JoystickZeroUp        , /*  KBD_JOY_UP     */
   JoystickZeroRight     , /*  KBD_JOY_RIGHT  */
   JoystickZeroDown      , /*  KBD_JOY_DOWN   */
   JoystickZeroLeft        /*  KBD_JOY_LEFT   */
  };

 int psp_kbd_mapping[ KBD_ALL_BUTTONS ];
 int psp_kbd_mapping_L[ KBD_ALL_BUTTONS ];
 int psp_kbd_mapping_R[ KBD_ALL_BUTTONS ];
 int psp_kbd_presses[ KBD_ALL_BUTTONS ];
 int kbd_ltrigger_mapping_active;
 int kbd_rtrigger_mapping_active;

 static int danzeff_atari_key     = 0;
 static int danzeff_atari_pending = 0;
 static int danzeff_mode        = 0;

# define KBD_MAX_ENTRIES   20

  int kbd_layout[KBD_MAX_ENTRIES][2] = {
    /* Key            Ascii */
    { ConsoleOn,                   -1 },
    { ConsoleOff,                  -1 },
    { ConsoleColor,                -1 },
    { ConsoleBlackWhite,           -1 },
    { ConsoleLeftDifficultyA,      DANZEFF_LDIFFA},
    { ConsoleLeftDifficultyB,      DANZEFF_LDIFFB},
    { ConsoleRightDifficultyA,     DANZEFF_RDIFFA },
    { ConsoleRightDifficultyB,     DANZEFF_RDIFFB },
    { ConsoleSelect,               DANZEFF_SEL },
    { ConsoleReset,                DANZEFF_RESET },
    { JoystickZeroUp,              -1 },
    { JoystickZeroDown,            -1 },
    { JoystickZeroLeft,            -1 },
    { JoystickZeroRight,           -1 },
    { JoystickZeroFire,            -1 },
    { JoystickOneUp,               -1 },
    { JoystickOneDown,             -1 },
    { JoystickOneLeft,             -1 },
    { JoystickOneRight,            -1 },
    { JoystickOneFire ,            -1 },
  };

# define PADDLE_MIN_RES     10000
# define PADDLE_STEP_RES    10000
# define PADDLE_MAX_RES   1000000

# define PADDLE_MIDDLE_RES 500000

  static int paddle_zero_resistance = PADDLE_MIDDLE_RES;
  static int paddle_one_resistance  = PADDLE_MIDDLE_RES;

static inline void
atari_inc_resistance(int *paddle, int step)
{
  *paddle = *paddle + step;

  if (*paddle < PADDLE_MIN_RES) *paddle = PADDLE_MIN_RES;
  if (*paddle > PADDLE_MAX_RES) *paddle = PADDLE_MAX_RES;
}

void
atari_paddle_event(int atari_idx)
{
  int step = ATARI.atari_paddle_speed * PADDLE_STEP_RES;

  if (! ATARI.atari_paddle_enable) return;

  if (ATARI.psp_active_joystick) {
    if ((atari_idx >= JoystickZeroUp) && 
        (atari_idx <= JoystickZeroFire)) {
      /* Convert Joystick Player 1 -> Player 2 ? */
      atari_idx = atari_idx - JoystickZeroUp + JoystickOneUp;
    }
  }
  /* Check Paddle */
  if (atari_idx == JoystickZeroLeft) {
    atari_inc_resistance(&paddle_zero_resistance, step);
    main_atari_send_paddle_event(PaddleZeroResistance, paddle_zero_resistance);
  } else
  if (atari_idx == JoystickZeroRight) {
    atari_inc_resistance(&paddle_zero_resistance, - step);
    main_atari_send_paddle_event(PaddleZeroResistance, paddle_zero_resistance);
  }

  if (atari_idx == JoystickOneLeft) {
    atari_inc_resistance(&paddle_one_resistance, step);
    main_atari_send_paddle_event(PaddleOneResistance, paddle_one_resistance);
  } else
  if (atari_idx == JoystickOneRight) {
    atari_inc_resistance(&paddle_one_resistance, - step);
    main_atari_send_paddle_event(PaddleOneResistance, paddle_one_resistance);
  }
}

int
atari_key_event(int atari_idx, int key_press)
{
  if (ATARI.psp_active_joystick) {
    if ((atari_idx >= JoystickZeroUp) && 
        (atari_idx <= JoystickZeroFire)) {
      /* Convert Joystick Player 1 -> Player 2 ? */
      atari_idx = atari_idx - JoystickZeroUp + JoystickOneUp;
    }
  }
  if ((atari_idx >= ATARIC_FPS) &&
      (atari_idx <= ATARIC_SCREEN)) {

    if (key_press) {
      gp2xCtrlData c;
      gp2xCtrlPeekBufferPositive(&c, 1);
      if ((c.TimeStamp - loc_last_hotkey_time) > KBD_MIN_HOTKEY_TIME) {
        loc_last_hotkey_time = c.TimeStamp;
        atari_treat_command_key(atari_idx);
      }
    }

  } else
  if ((atari_idx >=         0) && 
      (atari_idx < ATARI_MAX_KEY)) {
    main_atari_send_key_event(atari_idx, key_press);
    if (atari_idx == JoystickZeroFire) {
      main_atari_send_key_event(PaddleZeroFire, key_press);
    } else
    if (atari_idx == JoystickOneFire) {
      main_atari_send_key_event(PaddleOneFire, key_press);
    }
  }
  return 0;
}

int 
atari_kbd_reset()
{
  memset(loc_button_press  , 0, sizeof(loc_button_press));
  memset(loc_button_release, 0, sizeof(loc_button_release));

  paddle_one_resistance  = PADDLE_MIDDLE_RES;
  paddle_zero_resistance = PADDLE_MIDDLE_RES;

  return 0;
}

int
atari_get_key_from_ascii(int key_ascii)
{
  int index;
  for (index = 0; index < KBD_MAX_ENTRIES; index++) {
   if (kbd_layout[index][1] == key_ascii) return kbd_layout[index][0];
  }
  return -1;
}

void
psp_kbd_default_settings()
{
  memcpy(psp_kbd_mapping, loc_default_mapping, sizeof(loc_default_mapping));
  memcpy(psp_kbd_mapping_L, loc_default_mapping_L, sizeof(loc_default_mapping_L));
  memcpy(psp_kbd_mapping_R, loc_default_mapping_R, sizeof(loc_default_mapping_R));
}

int
psp_kbd_reset_hotkeys(void)
{
  int index;
  int key_id;
  for (index = 0; index < KBD_ALL_BUTTONS; index++) {
    key_id = loc_default_mapping[index];
    if ((key_id >= ATARIC_FPS) && (key_id <= ATARIC_SCREEN)) {
      psp_kbd_mapping[index] = key_id;
    }
    key_id = loc_default_mapping_L[index];
    if ((key_id >= ATARIC_FPS) && (key_id <= ATARIC_SCREEN)) {
      psp_kbd_mapping_L[index] = key_id;
    }
    key_id = loc_default_mapping_R[index];
    if ((key_id >= ATARIC_FPS) && (key_id <= ATARIC_SCREEN)) {
      psp_kbd_mapping_R[index] = key_id;
    }
  }
  return 0;
}

int
psp_kbd_load_mapping_file(FILE *KbdFile)
{
  char     Buffer[512];
  char    *Scan;
  int      tmp_mapping[KBD_ALL_BUTTONS];
  int      tmp_mapping_L[KBD_ALL_BUTTONS];
  int      tmp_mapping_R[KBD_ALL_BUTTONS];
  int      atari_key_id = 0;
  int      kbd_id = 0;

  memcpy(tmp_mapping, loc_default_mapping, sizeof(loc_default_mapping));
  memcpy(tmp_mapping_L, loc_default_mapping_L, sizeof(loc_default_mapping_L));
  memcpy(tmp_mapping_R, loc_default_mapping_R, sizeof(loc_default_mapping_R));

  while (fgets(Buffer,512,KbdFile) != (char *)0) {
      
      Scan = strchr(Buffer,'\n');
      if (Scan) *Scan = '\0';
      /* For this #@$% of windows ! */
      Scan = strchr(Buffer,'\r');
      if (Scan) *Scan = '\0';
      if (Buffer[0] == '#') continue;

      Scan = strchr(Buffer,'=');
      if (! Scan) continue;
    
      *Scan = '\0';
      atari_key_id = atoi(Scan + 1);

      for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++) {
        if (!strcasecmp(Buffer,loc_button_name[kbd_id])) {
          tmp_mapping[kbd_id] = atari_key_id;
          //break;
        }
      }
      for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++) {
        if (!strcasecmp(Buffer,loc_button_name_L[kbd_id])) {
          tmp_mapping_L[kbd_id] = atari_key_id;
          //break;
        }
      }
      for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++) {
        if (!strcasecmp(Buffer,loc_button_name_R[kbd_id])) {
          tmp_mapping_R[kbd_id] = atari_key_id;
          //break;
        }
      }
  }

  memcpy(psp_kbd_mapping, tmp_mapping, sizeof(psp_kbd_mapping));
  memcpy(psp_kbd_mapping_L, tmp_mapping_L, sizeof(psp_kbd_mapping_L));
  memcpy(psp_kbd_mapping_R, tmp_mapping_R, sizeof(psp_kbd_mapping_R));
  
  return 0;
}

int
psp_kbd_load_mapping(char *kbd_filename)
{
  FILE    *KbdFile;
  int      error = 0;
  
  KbdFile = fopen(kbd_filename, "r");
  error   = 1;

  if (KbdFile != (FILE*)0) {
  psp_kbd_load_mapping_file(KbdFile);
  error = 0;
    fclose(KbdFile);
  }

  kbd_ltrigger_mapping_active = 0;
  kbd_rtrigger_mapping_active = 0;
    
  return error;
}

int
psp_kbd_save_mapping(char *kbd_filename)
{
  FILE    *KbdFile;
  int      kbd_id = 0;
  int      error = 0;

  KbdFile = fopen(kbd_filename, "w");
  error   = 1;

  if (KbdFile != (FILE*)0) {

    for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++)
    {
      fprintf(KbdFile, "%s=%d\n", loc_button_name[kbd_id], psp_kbd_mapping[kbd_id]);
    }
    for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++)
    {
      fprintf(KbdFile, "%s=%d\n", loc_button_name_L[kbd_id], psp_kbd_mapping_L[kbd_id]);
    }
    for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++)
    {
      fprintf(KbdFile, "%s=%d\n", loc_button_name_R[kbd_id], psp_kbd_mapping_R[kbd_id]);
    }
    error = 0;
    fclose(KbdFile);
  }

  return error;
}

int 
psp_kbd_is_danzeff_mode()
{
  return danzeff_mode;
}

int
psp_kbd_enter_danzeff()
{
  unsigned int danzeff_key = 0;
  int          atari_key   = 0;
  int          key_code    = ConsoleSelect;
  gp2xCtrlData  c;

  if (! danzeff_mode) {
    psp_init_keyboard();
    danzeff_mode = 1;
  }

  gp2xCtrlPeekBufferPositive(&c, 1);

  if (danzeff_atari_pending) 
  {
    if ((c.TimeStamp - loc_last_event_time) > KBD_MIN_PENDING_TIME) {
      loc_last_event_time = c.TimeStamp;
      danzeff_atari_pending = 0;
      atari_key_event(danzeff_atari_key, 0);
    }

    return 0;
  }

  if ((c.TimeStamp - loc_last_event_time) > KBD_MIN_DANZEFF_TIME) {
    loc_last_event_time = c.TimeStamp;
  
    gp2xCtrlPeekBufferPositive(&c, 1);
    danzeff_key = danzeff_readInput(c);
  }

  if (danzeff_key > DANZEFF_START) {
    atari_key = atari_get_key_from_ascii(danzeff_key);

    if (atari_key != -1) {
      danzeff_atari_key     = atari_key;
      danzeff_atari_pending = 1;
      atari_key_event(danzeff_atari_key, 1);
    }

    return 1;

  } else if (danzeff_key == DANZEFF_START) {
    danzeff_mode          = 0;
    danzeff_atari_pending = 0;
    danzeff_atari_key     = 0;

    psp_kbd_wait_no_button();

  } else if (danzeff_key == DANZEFF_SELECT) {
    danzeff_mode          = 0;
    danzeff_atari_pending = 0;
    danzeff_atari_key     = 0;
    psp_main_menu();
    psp_init_keyboard();

    psp_kbd_wait_no_button();
  }

  return 0;
}

int
atari_decode_key(int psp_b, int button_pressed)
{
  int wake = 0;
  int reverse_cursor = ! ATARI.psp_reverse_analog;

  if (reverse_cursor) {
    if ((psp_b >= KBD_JOY_UP  ) &&
        (psp_b <= KBD_JOY_LEFT)) {
      psp_b = psp_b - KBD_JOY_UP + KBD_UP;
    } else
    if ((psp_b >= KBD_UP  ) &&
        (psp_b <= KBD_LEFT)) {
      psp_b = psp_b - KBD_UP + KBD_JOY_UP;
    }
  }

  if (psp_b == KBD_START) {
     if (button_pressed) psp_kbd_enter_danzeff();
  } else
  if (psp_b == KBD_SELECT) {
    if (button_pressed) {
      psp_main_menu();
      psp_init_keyboard();
    }
  } else {
 
    if (psp_kbd_mapping[psp_b] >= 0) {
      wake = 1;
      if (button_pressed) {
        // Determine which buton to press first (ie which mapping is currently active)
        if (kbd_ltrigger_mapping_active) {
          // Use ltrigger mapping
          psp_kbd_presses[psp_b] = psp_kbd_mapping_L[psp_b];
          atari_key_event(psp_kbd_presses[psp_b], button_pressed);
        } else
        if (kbd_rtrigger_mapping_active) {
          // Use rtrigger mapping
          psp_kbd_presses[psp_b] = psp_kbd_mapping_R[psp_b];
          atari_key_event(psp_kbd_presses[psp_b], button_pressed);
        } else {
          // Use standard mapping
          psp_kbd_presses[psp_b] = psp_kbd_mapping[psp_b];
          atari_key_event(psp_kbd_presses[psp_b], button_pressed);
        }
      } else {
          // Determine which button to release (ie what was pressed before)
          atari_key_event(psp_kbd_presses[psp_b], button_pressed);
      }

    } else {
      if (psp_kbd_mapping[psp_b] == KBD_LTRIGGER_MAPPING) {
        kbd_ltrigger_mapping_active = button_pressed;
        kbd_rtrigger_mapping_active = 0;
      } else
      if (psp_kbd_mapping[psp_b] == KBD_RTRIGGER_MAPPING) {
        kbd_rtrigger_mapping_active = button_pressed;
        kbd_ltrigger_mapping_active = 0;
      }
    }
  }
  return 0;
}

int
atari_decode_paddle(int psp_b)
{
  int reverse_cursor = ! ATARI.psp_reverse_analog;

  if (reverse_cursor) {
    if ((psp_b >= KBD_JOY_UP  ) &&
        (psp_b <= KBD_JOY_LEFT)) {
      psp_b = psp_b - KBD_JOY_UP + KBD_UP;
    } else
    if ((psp_b >= KBD_UP  ) &&
        (psp_b <= KBD_LEFT)) {
      psp_b = psp_b - KBD_UP + KBD_JOY_UP;
    }
  }

  if (psp_kbd_mapping[psp_b] != -1) {
    atari_paddle_event(psp_kbd_mapping[psp_b]);
  }
  return 0;
}

void
kbd_change_auto_fire(int auto_fire)
{
  ATARI.atari_auto_fire = auto_fire;
  if (ATARI.atari_auto_fire_pressed) {
    if (ATARI.psp_active_joystick) {
      atari_key_event(JoystickOneFire, 0);
    } else {
      atari_key_event(JoystickZeroFire, 0);
    }
    ATARI.atari_auto_fire_pressed = 0;
  }
}


static int 
kbd_reset_button_status(void)
{
  int b = 0;
  /* Reset Button status */
  for (b = 0; b < KBD_MAX_BUTTONS; b++) {
    loc_button_press[b]   = 0;
    loc_button_release[b] = 0;
  }
  psp_init_keyboard();
  return 0;
}

int
kbd_scan_keyboard(void)
{
  gp2xCtrlData c;
  long        delta_stamp;
  int         event;
  int         b;

  event = 0;
  gp2xCtrlPeekBufferPositive( &c, 1 );

  if (ATARI.atari_auto_fire) {
    delta_stamp = c.TimeStamp - first_time_auto_stamp;
    if ((delta_stamp < 0) || 
        (delta_stamp > (KBD_MIN_AUTOFIRE_TIME / (1 + ATARI.atari_auto_fire_period)))) {
      first_time_auto_stamp = c.TimeStamp;

      if (ATARI.psp_active_joystick) {
        atari_key_event(JoystickOneFire, ATARI.atari_auto_fire_pressed);
      } else {
        atari_key_event(JoystickZeroFire, ATARI.atari_auto_fire_pressed);
      }
      ATARI.atari_auto_fire_pressed = ! ATARI.atari_auto_fire_pressed;
    }
  }

  for (b = 0; b < KBD_MAX_BUTTONS; b++) 
  {
    if (c.Buttons & loc_button_mask[b]) {

      atari_decode_paddle(b);

      if (!(loc_button_data.Buttons & loc_button_mask[b])) {
        loc_button_press[b] = 1;
        event = 1;
      }
    } else {
      if (loc_button_data.Buttons & loc_button_mask[b]) {
        loc_button_release[b] = 1;
        event = 1;
      }
    }
  }
  memcpy(&loc_button_data,&c,sizeof(gp2xCtrlData));

  return event;
}

void
kbd_wait_start(void)
{
  while (1)
  {
    gp2xCtrlData c;
    gp2xCtrlReadBufferPositive(&c, 1);
    if (c.Buttons & GP2X_CTRL_START) break;
  }
  psp_kbd_wait_no_button();
}

void
psp_init_keyboard(void)
{
  atari_kbd_reset();
  kbd_ltrigger_mapping_active = 0;
  kbd_rtrigger_mapping_active = 0;
}

void
psp_kbd_wait_no_button(void)
{
  gp2xCtrlData c;

  do {
   gp2xCtrlPeekBufferPositive(&c, 1);
  } while (c.Buttons != 0);
} 

void
psp_kbd_wait_button(void)
{
  gp2xCtrlData c;

  do {
   gp2xCtrlReadBufferPositive(&c, 1);
  } while (c.Buttons == 0);
} 

int
psp_update_keys(void)
{
  int         b;
  gp2xCtrlData  c;

  static char first_time = 1;
  static int release_pending = 0;

  if (first_time) {

    gp2xCtrlPeekBufferPositive(&c, 1);
    if (first_time_stamp == -1) first_time_stamp = c.TimeStamp;

    first_time      = 0;
    release_pending = 0;

    for (b = 0; b < KBD_MAX_BUTTONS; b++) {
      loc_button_release[b] = 0;
      loc_button_press[b] = 0;
    }
    gp2xCtrlPeekBufferPositive(&loc_button_data, 1);

    psp_main_menu();
    psp_init_keyboard();

    return 0;
  }

  atari_apply_cheats();

  if (danzeff_mode) {
    psp_kbd_enter_danzeff();
    return 0;
  }

  if (release_pending)
  {
    release_pending = 0;
    for (b = 0; b < KBD_MAX_BUTTONS; b++) {
      if (loc_button_release[b]) {
        loc_button_release[b] = 0;
        loc_button_press[b] = 0;
        atari_decode_key(b, 0);
      }
    }
  }

  kbd_scan_keyboard();

  /* check press event */
  for (b = 0; b < KBD_MAX_BUTTONS; b++) {
    if (loc_button_press[b]) {
      loc_button_press[b] = 0;
      release_pending     = 0;
      atari_decode_key(b, 1);
    }
  }
  /* check release event */
  for (b = 0; b < KBD_MAX_BUTTONS; b++) {
    if (loc_button_release[b]) {
      release_pending = 1;
      break;
    }
  }

  return 0;
}
