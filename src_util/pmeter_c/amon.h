#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/*  1992.1.23 */

#define _SIZE_T
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include "swin.h"

struct average_runtime_monitor {
    struct window_object *window;
    int count;
    unsigned int scroll;
    unsigned int cycle;
    int displayType;
    int sx;
    int sy;
    unsigned int sw;
    unsigned int sh;
    unsigned int width;
    unsigned int height;
    char *color[5];
};

struct ainit_info {
    int	x;
    int y;
    unsigned int SWidth;
    unsigned int SHeight;
    unsigned int width;
    unsigned int height;
    unsigned int scroll;
    unsigned int cycle;
    int	displayType;
    char *title;
};

typedef struct average_runtime_monitor Amon;
typedef struct ainit_info AInit;

extern void draw_amonitor();
extern void resetAinit();
extern Amon *create_amonitor();
extern void deactivate_amonitor();
extern void kill_amonitor();

