#ifndef _ATARI_H_
#define _ATARI_H_

# ifdef __cplusplus
extern "C" {
# endif

//LUDO:
# define ATARI_RENDER_NORMAL   0
# define ATARI_LAST_RENDER     0

# define ATARI_FLICKER_NONE      0
# define ATARI_FLICKER_SIMPLE    1
# define ATARI_FLICKER_AVERAGE   2
# define ATARI_FLICKER_PHOSPHOR  3
# define ATARI_LAST_FLICKER      3

# define MAX_PATH           256
# define ATARI_MAX_SAVE_STATE 5
# define ATARI_MAX_CHEAT        10

#include <SDL/SDL.h>

#define ATARI_CHEAT_NONE    0
#define ATARI_CHEAT_ENABLE  1
#define ATARI_CHEAT_DISABLE 2

#define ATARI_RAM_SIZE  0x10000

#define ATARI_CHEAT_COMMENT_SIZE 25
  
  typedef struct Atari_cheat_t {
    unsigned char  type;
    unsigned short addr;
    unsigned char  value;
    char           comment[ATARI_CHEAT_COMMENT_SIZE];
  } Atari_cheat_t;


  typedef struct Atari_save_t {

    SDL_Surface    *surface;
    char            used;
    char            thumb;
    time_t          date;

  } Atari_save_t;

  typedef struct Atari_t {
 
    Atari_save_t atari_save_state[ATARI_MAX_SAVE_STATE];
    Atari_cheat_t atari_cheat[ATARI_MAX_CHEAT];

    int  comment_present;
    char atari_save_name[MAX_PATH];
    char atari_home_dir[MAX_PATH];
    int  psp_screenshot_id;
    int  psp_cpu_clock;
    int  psp_reverse_analog;
    int  atari_current_clock;
    int  atari_view_fps;
    int  atari_current_fps;
    int  psp_active_joystick;
    int  atari_flicker_mode;
    int  atari_snd_enable;
    int  atari_render_mode;
    int  atari_speed_limiter;
    int  psp_skip_max_frame;
    int  psp_skip_cur_frame;
    int  atari_slow_down_max;
    int  atari_paddle_enable;
    int  atari_paddle_speed;
    int  atari_auto_fire;
    int  atari_auto_fire_pressed;
    int  atari_auto_fire_period;

  } Atari_t;

  extern Atari_t ATARI;

  extern int psp_exit_now;

  extern void main_atari_send_key_event(int atari_idx, int key_press);
  extern int  main_atari_load_state(char *filename);
  extern void main_atari_force_draw_blit();
  extern int  main_atari_save_state(char *filename);
  extern int main_atari_load_rom(char *filename);
  extern void main_atari_emulator_reset();
  extern int main_atari_cheat_poke(int addr, unsigned char value);
  extern int main_atari_cheat_peek(int addr, unsigned char* p_result);

# ifdef __cplusplus
}
# endif

#endif
