#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* 1992.1.23 */
#ifndef _SWin_Included
#define _SWin_Included

#include <X11/Xlib.h>

#define   YES     1     /*reshape*/
#define   NO      0     /*reshape*/ 
#define   HAVE    1 /*decoration window */
#define   NOHAVE  0 /*decoration window */

#define   pSet    1     /*line opration */
#define   pReset  2     /*line opration */
#define   pExor   3     /*line opration */
#define   pSolid  1     /*line type */
#define   pAlternate 2   /*line type */
#define   ACTIVATE   1   /*window activity */
#define   DEACTIVATE 0   /*window activity */

struct  window_object {
    Display *display;
    int     screen_number;
    Window window;
    unsigned long border;
    unsigned long background;
    GC  gc;
    Font font;
    Pixmap pixmap;
    Cursor cursor;
    Window      parent;
    char        *color_name;
    int         decoration_win;
    int  cursor_x;
    int  cursor_y;
    int  activity
};

struct window_init_info {
    int  x;
    int  y;
    unsigned int width;
    unsigned int height;
    unsigned int border_width;
    char *border_color;
    char *back_color;
    struct window_object *parent;
    int  cursor_name;
    char *font_name;
    char *title;
    int min_width;
    int min_height;
    int width_inc;
    int height_inc;
};

typedef struct window_object SWin;
typedef struct window_init_info InitInfo;
typedef   int     Optype;
typedef   int     Linetype;

extern SWin *createSWin();
extern void get_position();
extern void set_position();
extern void draw_string();
extern void draw_line();
extern void draw_rectangle();
extern void draw_filled_rectangle();
extern void deactivateSWin();
extern void activateSWin();
extern void killSWin();
extern void set_color();
extern void set_cursor();
extern int  read_click();
extern char *get_l();
extern char get_c();
extern void resetWinit();
extern void clear_area();
extern void transfer();
extern void flush();
extern int get_reshape();
extern void get_size();
extern void put_size();
extern void get_font_size();
extern void set_font();
extern void draw_pattern_rectangle();
#endif
