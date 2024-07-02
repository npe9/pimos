#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* 1992.3.5 */

#define _SIZE_T
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "swin.h"

#define   MAXCHAR 255           /* get_l max charctor */

static int xop_type();
static int xline_type();

/********** resetWinit **********/
void resetWinit(wInfo)
InitInfo *wInfo;
{
    wInfo->x = 0;
    wInfo->y = 0;
    wInfo->width = 0;
    wInfo->height = 0;
    wInfo->border_width = 1;
    wInfo->border_color = (char *)NULL;
    wInfo->back_color = (char *)NULL;
    wInfo->parent = (SWin *)NULL;
    wInfo->cursor_name = (int)NULL;
    wInfo->font_name = (char *)NULL;
    wInfo->title = "No Name";
    wInfo->min_width=1;
    wInfo->min_height=1;
    wInfo->width_inc=1;
    wInfo->height_inc=1;
}

/********** CreateSWin **********/
SWin *createSWin(w_info)
InitInfo w_info;
{
    SWin *swin;
    Display  *display;
    int  screen_number;
    Window win;
    Window parent;
    unsigned long background;
    unsigned long border;
    GC   gc;
    Cursor      cursor;
    Font font;
    XSizeHints myhints;
    int x;
    int y;
    unsigned int width;
    unsigned int height;
    unsigned int border_width;
    char *border_color;
    char *back_color;
    Colormap cmap;
    XColor      borderColor;
    XColor  backColor;
    XColor color;     /* dammy */
    XSetWindowAttributes  attri;

    x = w_info.x;
    y = w_info.y;
    width = w_info.width;
    height = w_info.height;
    border_width = w_info.border_width;
    border_color = w_info.border_color;
    back_color = w_info.back_color;

    display = XOpenDisplay(NULL);
    screen_number = DefaultScreen(display);

    if(w_info.parent == (SWin *)NULL){
       parent = RootWindow(display,screen_number);
    }else{
       parent = w_info.parent->window;
    }

    cmap = DefaultColormap(display,screen_number);

    if (border_color == (char *)NULL){
         border = BlackPixel(display,screen_number);
    }else{
         XAllocNamedColor(display,cmap,border_color,&borderColor,&color);
         border = borderColor.pixel;
    }


    if (back_color == (char *)NULL){
        background = WhitePixel(display,screen_number);
    }else{
        XAllocNamedColor(display,cmap,back_color,&backColor,&color);
        background = backColor.pixel;
    }


    win = XCreateSimpleWindow(display,parent,x,y,width,height,border_width,border,background);


    attri.backing_store = Always;
    attri.bit_gravity = NorthWestGravity;
    XChangeWindowAttributes(display,win,CWBackingStore|CWBitGravity,&attri);
    myhints.x = x;
    myhints.y = y;
    myhints.min_width = w_info.min_width;
    myhints.min_height = w_info.min_height;
    myhints.width_inc = w_info.width_inc;
    myhints.height_inc = w_info.height_inc;
    myhints.flags = PMinSize | PResizeInc|USPosition;
    XSetNormalHints(display,win,&myhints);
    XStoreName(display,win,w_info.title);


    if(w_info.cursor_name != (int)NULL){
 cursor = XCreateFontCursor(display,w_info.cursor_name);
 XDefineCursor(display,win,cursor);
    }
    gc = XCreateGC(display,win,0,0);
    XSetForeground(display,gc,border);
    XSetBackground(display,gc,background);

/*    gcvalue.graphics_exposures=False;
    XChangeGC(display,gc,GCGraphicsExposures,&gcvalue);
*/
    XSetGraphicsExposures(display,gc,False);

/*    XSelectInput(display,win,ExposureMask|ButtonPressMask
   |ButtonReleaseMask|OwnerGrabButtonMask
   |KeyPressMask|StructureNotifyMask);
   
*/
    XSelectInput(display,win,StructureNotifyMask);


    if(w_info.font_name != (char *)NULL){
 font = XLoadFont(display,w_info.font_name);
 XSetFont(display,gc,font);
    }else{
 font = XLoadFont(display,"a14");
 XSetFont(display,gc,font);
    }


    swin = (SWin *)malloc(sizeof(SWin));


    swin->display = display;
    swin->screen_number = screen_number;
    swin->window = win;
    swin->border = border;
    swin->background = background;
    swin->gc = gc;
    swin->font = font;
     swin->pixmap = (Pixmap)NULL;
    swin->cursor = cursor;
    swin->parent = parent;
    swin->activity = DEACTIVATE;
    swin->cursor_x = 0;
    swin->cursor_y = 0;
    return(swin);
}

/********** activateSWin **********/

void activateSWin(win)
SWin *win;
{
    Display *display;
    Window  window;
    Window  parent;
    Window  parent2;
    Window  root;           /* dummy */
    Window  *children;      /* dummy */
    unsigned int nchildren; /* dummy */
    XEvent  event;          /* dummy */
    int eventIn;

    display = win -> display;
    window  = win -> window;
    parent2 = win -> parent;

    if(win->activity != ACTIVATE){

      XMapRaised(display,window);

      eventIn = False;
      while(!eventIn){
        XNextEvent(display,&event);
        if(event.type == MapNotify && event.xmap.event == window)
          eventIn = True;
      }
      win -> activity = ACTIVATE;
      XQueryTree(display,window,&root,&parent,&children,&nchildren); /*get parent*/


      if(nchildren>0) XFree(children);


      if(parent == parent2){
         win->decoration_win = NOHAVE;
        }else{
      win->decoration_win = HAVE;
      }
    }

}

/********** deactivateSWin **********/

void deactivateSWin(win)
SWin *win;
{
    Display *display;
    Window  window;
    int eventIn;
    XEvent event;
    display = win -> display;
    window  = win -> window;
    if(win->activity == ACTIVATE){
      XUnmapWindow(display,window);
      eventIn = False;
      while(!eventIn){
        XNextEvent(display,&event);
        if(event.type == UnmapNotify && event.xunmap.event == window)
          eventIn = True;
        }
      win -> activity = DEACTIVATE;
      XFlush(display);
    }
}

/********** get_position **********/

void get_position(win,x,y)
SWin *win;
int  *x;
int  *y;
{
    Display *display;
    Window  root;
    Window  window;
    Window  parent;
    Window  *children; /* dummy */
    
    int     decoration_win;
    int     x1;
    int     y1;
    unsigned int width;     /* dummy */
    unsigned int height;     /* dummy */
    unsigned int border;    /* dummy */
    unsigned int depth;     /* dummy */
    unsigned int nchildren; /* dummy */

    display = win -> display;
    window  = win -> window;
    decoration_win = win -> decoration_win;

    if (decoration_win == HAVE){
        XQueryTree(display,window,&root,&parent,&children,&nchildren);/*get parent */
 XGetGeometry(display,parent,&root,&x1,&y1,&width,&height,&border,&depth); /* get x,y */
    }else{
 XGetGeometry(display,window,&root,&x1,&y1,&width,&height,&border,&depth); /* get x,y */
    }

    if(nchildren>0) XFree(children);
    *x = x1;
    *y = y1;
}

/********** set_position **********/

void set_position(win,x,y)
SWin *win;
int  x;
int  y;
{
    Display  *display;
    Window   window;
    Window   parent;
    Window   root;      /* dummy */
    Window   *children; /* dummy */
    int     x1;
    int     y1;
    unsigned int width;      /* dummy */
    unsigned int height;      /* dummy */
    unsigned int border;     /* dummy */
    unsigned int depth;      /* dummy */
    unsigned int nchildren;  /* dummy */
    int      decoration_win; /* dummy */

    decoration_win = win -> decoration_win;
    display = win -> display;
    window  = win -> window;
    
    if (decoration_win == HAVE){
      XQueryTree(display,window,&root,&parent,&children,&nchildren); /* get parent */
      XGetGeometry(display,parent,&root,&x1,&y1,&width,&height,&border,&depth); /* get border */
      XMoveWindow(display,parent,x+ (int)border,y+ (int)border);
    }else{
       XMoveWindow(display,window,x,y);
    }
    if(nchildren>0) XFree(children);
    XFlush(display);
}
/********** get_size **********/
void get_size(win,width,height)
SWin *win;
unsigned int *width;
unsigned int *height;
{
    Display *display;
    Window window;
    Window root;
    int x;
    int y;
    unsigned int width2;
    unsigned int height2;
    unsigned int border;
    unsigned int depth;

    display = win->display;
    window = win->window;
   
    XGetGeometry(display,window,&root,&x,&y,&width2,&height2,&border,&depth);
    *width = width2;
    *height = height2;
}

/********** set_size **********/
void set_size(win,width,height)
SWin *win;
unsigned int width;
unsigned int height;
{
    Display *display;
    Window window;

    display = win->display;
    window = win->window;

    XResizeWindow(display,window,width,height);
}

/********** draw_string **********/

void draw_string(win,x,y,data,font_name,op)
SWin *win;
int x;
int y;
char *data;
char *font_name;
Optype op;
{
    GC  gc;
    Font font;
    Display  *display;
    Window window;
    int charW;
    int charH;

    display = win -> display;
    window  = win -> window;
    gc      = win -> gc;

    font = XLoadFont(display,font_name);
    XSetFont(display,gc,font);
    win->font = font;
    XSetFunction(display,gc,xop_type(op));
    get_font_size(win,&charW,&charH);
    XDrawString(display,window,gc,x,y+charH,data,strlen(data));
    XFlush(display);
}

/********** draw_line **********/

void draw_line(win,x1,y1,x2,y2,line_width,line_type,op)
SWin *win;
int x1;
int x2;
int y1;
int y2;
Linetype line_type;
Optype op;
unsigned int line_width;
{
    Display  *display;
    Window window;
    GC  gc;

    display = win -> display;
    window  = win -> window;
    gc     = win -> gc;

    XSetFunction(display,gc,xop_type(op));
    XSetLineAttributes(display,gc,line_width,xline_type(line_type),CapButt,JoinMiter);
    XDrawLine(display,window,gc,x1,y1,x2,y2);
    XFlush(display);
}

/********** draw_rectangle **********/

void draw_rectangle(win,x,y,width,height,line_width,line_type,op)
SWin *win;
int x;
int y;
Linetype line_type;
unsigned int width;
unsigned int height;
unsigned int line_width;

Optype op;
{
    Display  *display;
    Window window;
    GC  gc;

    display = win -> display;
    window  = win -> window;
    gc     = win -> gc;

    XSetLineAttributes(display,gc,line_width,xline_type(line_type),CapButt,JoinMiter);
    XSetFunction(display,gc,xop_type(op));

    XDrawRectangle(display,window,gc,x,y,width,height);
    XFlush(display);
}

/********** draw_filled_rectangle **********/
void draw_filled_rectangle(win,x,y,width,height,line_width,pattern,op)
SWin *win;
int x;
int y;
unsigned int width;
unsigned int height;
unsigned int line_width;
char         *pattern;
Optype op;
{
    Display  *display;
    Window window;
    GC  gc;
    Pixmap draw_pattern;

    display = win -> display;
    window  = win -> window;
    gc     = win -> gc;
    if(line_width == 0){
      x = x +1;
      y = y +1;
      width = width -2;
      height = height -2;
    }
    if(pattern!=NULL){
      draw_pattern = XCreateBitmapFromData(display,window,pattern,16,16);
      XSetStipple(display,gc,draw_pattern);
      XSetFillStyle(display,gc,FillStippled);
    }
    XSetFunction(display,gc,xop_type(op));
    XFillRectangle(display,window,gc,x,y,width,height);
    XSetFillStyle(display,gc,FillSolid);
    XFlush(display);    
}
/********** draw_pattern_rectangle **********/
void draw_pattern_rectangle
  (win,x,y,width,height,line_width,pattern,patternW,patternH,op)
SWin *win;
int x;
int y;
unsigned int width;
unsigned int height;
unsigned int line_width;
char         *pattern;
unsigned int patternW;
unsigned int patternH;
Optype op;
{
    Display  *display;
    Window window;
    GC  gc;
    Pixmap draw_pattern;

    display = win -> display;
    window  = win -> window;
    gc     = win -> gc;
    if(line_width == 0){
      x = x +1;
      y = y +1;
      width = width -2;
      height = height -2;
    }
    if(pattern!=NULL){
      draw_pattern = XCreateBitmapFromData
	(display,window,pattern,patternW,patternH);
      XSetStipple(display,gc,draw_pattern);
      XSetFillStyle(display,gc,FillStippled);
    }
    XSetFunction(display,gc,xop_type(op));
    XFillRectangle(display,window,gc,x,y,width,height);
    XSetFillStyle(display,gc,FillSolid);
    XFlush(display);    
}

/********** killSWin **********/

void killSWin(win)
SWin *win;
{
    Display  *display;
    Window window;
    GC  gc;

    display = win -> display;
    window  = win -> window;
    gc      = win -> gc;   
    if(win->activity == ACTIVATE) deactivateSWin(win);
    XDestroyWindow(display,window);
    XFreeGC(display,gc);
    XFlush(display);
    XCloseDisplay(display);
    free(win);
}

/********** set_color **********/
void set_color(win,color_name)
SWin *win;
char *color_name;
{
    Display  *display;
    int  screen_number;
    GC   gc;
    Colormap    cmap;
    XColor      color;

    display = win->display;
    gc = win -> gc;
    win -> color_name = color_name;
    screen_number = win->screen_number;

    cmap = DefaultColormap(display,screen_number);
    XParseColor(display,cmap,color_name,&color);
    XAllocColor(display,cmap,&color);
    XSetForeground(display,gc,color.pixel);
}


/********** set_cursor **********/
void set_cursor(win,cursor_name)
SWin *win;
{
    Cursor cursor;
    Display *display;
    Window  window;

    window  = win -> window;
    display = win -> display;

    cursor = XCreateFontCursor(display,cursor_name);
    XDefineCursor(display,window,cursor);
    XFlush(display);
}

/********** read_click **********/
int read_click(win,x,y)
SWin *win;
int  *x;
int  *y;
{
    XEvent event;
    Window window;
    Display *display;
    int eventIn;

    window = win -> window;
    display = win -> display;

    eventIn = False;
    while(!eventIn){
      XNextEvent(display,&event);
      if(event.type == ButtonPress && event.xbutton.window == window)
        eventIn = True;
    }
    eventIn = False;
    while(!eventIn){
      XNextEvent(display,&event);
      if(event.type == ButtonRelease && event.xbutton.window == window)
        eventIn = True;
    }
    *x = event.xbutton.x;
    *y = event.xbutton.y;
    return(event.xbutton.button);
}

/********** get_l **********/

char *get_l(win)
SWin *win;
{
    XFontStruct *font_struct;
    Font font_id;
    Display *display;
    int  cursor_x = 0;
    int  cursor_y = 0;
    int  x;  /* dummy */
    int  y;  /* dummy */
    Window window;
    Window root;
    XEvent event;
    char str[10];
    int  len = 10;
    KeySym keysym;
    int  p = 0;
    int  p2 = 0;
    char ptext[MAXCHAR];
    int  isend = False;
    int         eventIn;
    int  maxOver = False;
    GC  gc;
    char  *str1;
    char  *str2;
    unsigned int width;
    unsigned int height;
    unsigned int depth;        /* dummy */
    unsigned int border_width; /* dummy */

    font_id = win -> font;
    display = win -> display;
    window  = win -> window;
    gc      = win -> gc;
    cursor_y = win -> cursor_y;

    XGetGeometry(display,window,&root,&x,&y,&width,&height,&border_width,&depth);/* get width height */
    font_struct = XQueryFont(display,font_id);
    if (cursor_y + font_struct->max_bounds.ascent +
	font_struct->max_bounds.descent > height){
      cursor_y = 0;
    }
    cursor_y = cursor_y + font_struct->max_bounds.ascent;
    while(!isend){
      eventIn = False;
      while(!eventIn){
	XNextEvent(display,&event);
	if(event.type == KeyPress && event.xkey.window == window)
	  eventIn = True;
      }
      if(XLookupString(&event,str,len,&keysym,NULL)==1){
	if(keysym == XK_Return) isend = True;
	if(isprint(str[0])){
	  ptext[p] = str[0];
	  ++p;
	  str[1]='\0';
	  XDrawString(display,window,gc,cursor_x,cursor_y,str,1);
	  cursor_x = cursor_x +
	    font_struct->max_bounds.rbearing;
	  if(cursor_x +font_struct->max_bounds.rbearing >
	     width){
	    cursor_x = 0;
	    cursor_y = cursor_y +
	      font_struct->max_bounds.ascent +
		font_struct->max_bounds.descent;
	    if(cursor_y > height){
	      cursor_y = 0;
	      cursor_y = cursor_y +
		font_struct->max_bounds.ascent;
	    }
	  }
	}
	if(p==MAXCHAR || isend){
	  ptext[p] = '\0';
	  if(!maxOver){
	    p2 = p;
	    maxOver = True;
	    str1 = (char *)malloc(sizeof(char)*p);
	    strcpy(str1,ptext);
	    p=0;
	  }else{
	    p2 = p2+ p-1;
	    str2 = (char *)malloc(sizeof(char)*p2);
	    strcpy(str2,str1);
	    free(str1);
	    str1 = (char *)malloc(sizeof(char)*p2);
	    str1 = strcat(str2,ptext);
	    free(str2);
	    p = 0;
	  }
	}
      }
    }
    win -> cursor_y = cursor_y + font_struct->max_bounds.descent;
    return(str1);
}

/********** get_c **********/

char get_c(win)
SWin *win;
{
    Display *display;
    Window  window;
    XEvent  event;
    char    str[10];
    KeySym  keysym;
    int     len = 10;
    int     isend = False;
    int     eventIn;

    display = win -> display;
    window  = win -> window;
    
    while(!isend)
      {
	eventIn = False;
	while(!eventIn){
	  XNextEvent(display,&event);
	  if(event.type == KeyPress && event.xkey.window == window)
	    eventIn = True;
	}
	XLookupString(&event,str,len,&keysym,NULL);
	if(iscntrl(str[0]) || isprint(str[0])){
	  isend = True;
	} 
      }  
    return(str[0]);
}
/********** set_font **********/
void set_font(win,font_name)
SWin *win;
char *font_name;
{
    Font font;

    font = XLoadFont(win->display,font_name);
    XSetFont(win->display,win->gc,font);

    win->font = font;
}

/********** get_font_size **********/
void get_font_size(win,charW,charH)
SWin *win;
int *charW;
int *charH;
{
    XFontStruct *font_struct;

    font_struct = XQueryFont(win->display,win->font);
    *charW = font_struct -> max_bounds.rbearing;
    *charH = font_struct -> max_bounds.ascent + font_struct ->
      max_bounds.descent;
}

/********** transfer **********/
void transfer(win,x1,y1,w,h,x2,y2)
SWin *win;
int x1;
int y1;
int w;
int h;
int x2;
int y2;
{
    Display *display;
    Window  window;
    GC     gc;

    display = win -> display;
    window = win -> window;
    gc = win-> gc;

    XCopyArea(display,window,window,gc,x1,y1,w,h,x2,y2);
}    

/********** clear_area **********/
void clear_area(win,x,y,w,h)
SWin *win;
int x;
int y;
int w;
int h;
{
    Display *display;
    Window  window;
    int eventIn;
    XEvent  event;

    set_color(win,"white");
    draw_filled_rectangle(win,x,y,w,h,1,NULL,pSet);

}

/********** flush **********/
void flush(win)
SWin *win;
{
  XFlush(win->display);
}

/********** get_reshape **********/
int get_reshape(win,width,height)
SWin *win;
int *width;
int *height;
{
  XEvent event;
  int reshape = NO;
  int test;

  while(XEventsQueued(win->display,QueuedAfterReading)!=0){
    XNextEvent(win->display,&event);
    if(event.type == ConfigureNotify){
	*width = event.xconfigure.width;
	*height = event.xconfigure.height;
	reshape = YES;
      }
  }
  return(reshape);
}

extern void set_title(win,title)
SWin *win;
char *title;
{
  XStoreName(win->display,win->window,title);
}

/***************************/
/********** local **********/
/***************************/

/********** xline_type **********/
static int xline_type(line_type)
Linetype line_type;
{
    if (line_type == pSolid) return(LineSolid);
    if (line_type == pAlternate) return(LineOnOffDash);
    return(LineSolid);
}
/********** xop_type **********/
static int xop_type(op)
Optype op;
{
    if (op == pSet) return(GXcopy);
    if (op == pReset) return(GXclear);
    if (op == pExor) return(GXxor);
    return(GXcopy);
}
