#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#include "gp2x_psp.h"
#include "gp2x_cpu.h"

#include <time.h>
#include <dirent.h>

# ifndef CLK_TCK
# define CLK_TCK  CLOCKS_PER_SEC
# endif

#include "Atari.h"

# define ATARI_WIDTH  320
# define ATARI_HEIGHT 300
# define SNAP_WIDTH   140
# define SNAP_HEIGHT  100

#ifdef __cplusplus
}
#endif
#endif
