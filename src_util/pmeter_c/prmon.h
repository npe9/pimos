#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* 1992.4.29 */
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

struct p_runtime_monitor {
    struct window_object *window;
    Pixmap pixmap[41];
    unsigned int pe;
    unsigned int Cluster;
    unsigned int ClusterH;
    unsigned int ClusterW;
    int count;
    unsigned int scroll;
    unsigned int cycle;
    unsigned int type;
    unsigned int uneven;
    int display;
    int sx;
    int sy;
    unsigned int sw;
    unsigned int sh;
    unsigned int width;
    unsigned int height;
    char *Color[20];
};

struct p_rinit_info {
    int	x;
    int y;
    unsigned int width;
    unsigned int height;
    unsigned int scroll;
    unsigned int cycle;
    unsigned int type;
    unsigned int uneven;
    int	display;
    char *title;
};

typedef struct p_runtime_monitor PRmon;
typedef struct p_rinit_info PRInit;

extern void draw_prmon();
extern void resetPRinit();
extern PRmon *create_prmon();
extern void activate_prmon();
extern void deactivate_prmon();
extern void kill_prmon();

