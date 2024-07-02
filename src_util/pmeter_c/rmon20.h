#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* 1992.3.12 */
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

/* if not define white, white will define */
#ifndef white
#define white 1
#endif
/* if not define black, black will define */
#ifndef black
#define black 0
#endif

#ifndef convex
#define convex 0 /* totu gata */
#endif

#ifndef concave
#define concave 1 /* ou gata */
#endif

struct runtime_monitor {
    struct window_object *window;
    Pixmap pixmap[40];
    unsigned int pe;
    int count;
    unsigned int scroll;
    unsigned int cycle;
    unsigned int type;
    unsigned int uneven;
    int displayType;
    int sx;
    int sy;
    unsigned int sw;
    unsigned int sh;
    unsigned int width;
    unsigned int height;
    char *color[20];
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
    unsigned int type;
    unsigned int uneven;
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


