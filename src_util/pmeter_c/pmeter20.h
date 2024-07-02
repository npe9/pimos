#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* 1992.1.21 */

#define _SIZE_T

#include "swin.h"
#include "seg7.h"

/**** ICOT's Original Colors ****/
#ifdef OldColor
#define Pcolor0 "#fbf4fbf4fbf4"
#define Pcolor1 "#fbf4fbf47d00"
#define Pcolor2 "#fbf4fbf40000"
#define Pcolor3 "#7d00fbf40000"
#define Pcolor4 "#0000fbf40000"
#define Pcolor5 "#0000dea8dea8"
#define Pcolor6 "#00007d00fbf4"
#define Pcolor7 "#00000000fbf4"
#define Pcolor8 "#000000007d00"
#define Pcolor9 "#000000000000"

/*
  #define Pcolor0 "white"
  #define Pcolor1 "Khaki"
  #define Pcolor2 "yellow"
  #define Pcolor3 "OliveDrab1"
  #define Pcolor4 "green"
  #define Pcolor5 "LightSkyBlue"
  #define Pcolor6 "DeepSkyBlue"
  #define Pcolor7 "blue"
  #define Pcolor8 "blue4"
  #define Pcolor9 "black"
*/

#endif

/**** Runtime Monitor's Colors *****/
/*
#ifndef OldColor
#define Pcolor0 "#ffff00001388"; 
#define Pcolor1 "#fde84e200000"; 
#define Pcolor2 "#fde875302710"; 
#define Pcolor3 "#e600ea604e20"; 
#define Pcolor4 "#9c40ea602710"; 
#define Pcolor5 "#4e20c3504e20"; 
#define Pcolor6 "#00007530fb00"; 
#define Pcolor7 "#00000000fb00"; 
#define Pcolor8 "#27100000d8ef"; 
#define Pcolor9 "#000000007d00"; 
#endif
*/
#ifndef OldColor
#define Pcolor0 "red";
#define Pcolor1 "OrangeRed";
#define Pcolor2 "tomato";
#define Pcolor3 "coral";
#define Pcolor4 "DarkOrange";
#define Pcolor5 "orange";
#define Pcolor6 "gold";
#define Pcolor7 "#ffffeeee0000"; 
#define Pcolor8 "#ddddffff0000"; 
#define Pcolor9 "GreenYellow";  
#define Pcolor10 "LawnGreen";
#define Pcolor11 "green";
#define Pcolor12 "#1111ffff9999"; 
#define Pcolor13 "#0000ddddffff"; 
#define Pcolor14 "#0000bbbbffff"; 
#define Pcolor15 "#00009999ffff"; 
#define Pcolor16 "#00007777ffff"; 
#define Pcolor17 "#00005555ffff"; 
#define Pcolor18 "#00000000ffff"; 
#define Pcolor19 "MediumBlue"; 
#endif

struct pmeter {
   SWin   *window;
   int    pe;
   unsigned int width;
   unsigned int height;
   unsigned int pe_width;
   unsigned int pe_height;
   unsigned int pe_interval;
   unsigned int pe_box_margin;
   int x0;
   int y0;
   CountI *counter;
   Pixmap pixmap[20];
   int matrix_h;
   int matrix_w;
};

struct pinit_info {
    int	x;
    int y;
    unsigned int width;
    unsigned int height;
    char *title;
};

typedef struct pmeter Pmeter;
typedef struct pinit_info PInit;

extern Pmeter *create_pmeter();
extern void kill_pmeter();
extern void draw_pmeter();
extern void resetPinit();
