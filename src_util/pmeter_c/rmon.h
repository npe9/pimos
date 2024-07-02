#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* 1992.1.23 */
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

struct runtime_monitor {
    struct window_object *window;
    Pixmap pixmap[20];
    unsigned int pe;
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
    char *color[10];
};

struct rinit_info {
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

typedef struct runtime_monitor Rmon;
typedef struct rinit_info RInit;

extern void draw_rmonitor();
extern void resetRinit();
extern Rmon *create_rmonitor();
extern void activate_rmonitor();
extern void deactivate_rmonitor();
extern void kill_rmonitor();


