#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/*
   Counter for Pmeter
   1992.2.4
*/

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
#include "seg7.h"

static void draw_vertical();
static void draw_horizontal();
static void draw_pos_g();
static void draw_pos_f();
static void draw_pos_e();
static void draw_pos_d();
static void draw_pos_c();
static void draw_pos_b();
static void draw_pos_a();
static void draw_numerics();
static void clear_numerics();
static void change_layout_info();
static int num_length();
static void init_draw_indicator();

extern CountI *create_count_indicator(win,cInfo)
SWin *win;
CInfo cInfo;
{
    CountI *counter;
    int x1;
    int y1;
    unsigned int w1;
    unsigned int h1;
    unsigned int margin;

    counter=(CountI *)malloc( sizeof(CountI) );

    margin=cInfo.margin;
    counter->margin=margin;

    counter->x=cInfo.x;
    counter->y=cInfo.y;
    counter->width=cInfo.width;
    counter->height=cInfo.height;
    counter->length=cInfo.length;
    counter->line_width=cInfo.line_width;

    counter->x1=cInfo.x+margin;
    counter->y1=cInfo.y+margin;
    counter->w1=(cInfo.width-(cInfo.length+1)*margin)/
                (cInfo.length);
    counter->h1=cInfo.height-margin*2;

    counter->margin=margin;

    counter->window=win;

    counter->count=0;

    if(cInfo.layout_mode==1){ 
        counter->layout_mode=1;
    }else{
        counter->layout_mode=0;
    }

    init_draw_indicator(counter);

    draw_numerics(counter,1,0,-1);

    return(counter);

}

extern int set_counter(counter,value)
CountI *counter;
int value;
{
    int old;
    int new;
    int oldlen;
    int newlen;

    old=counter->count;
    counter->count=value;
    new=value;

    oldlen=num_length(old);
    newlen=num_length(new);

    if(oldlen>newlen) clear_numerics(counter,newlen+1,oldlen-newlen);
    draw_numerics(counter,newlen,new,old);

}

extern int reset_counter(counter)
CountI *counter;
{
    set_counter(counter,0);
}

extern int countup(counter)
CountI *counter;
{
    int count;
    count=counter->count+1;
    set_counter(counter,count);
}

extern void reshape_counter(counter,x,y,width,height)
CountI *counter;
int x;
int y;
unsigned int width;
unsigned int height;
{
    int count;

    count=counter->count;
    counter->x=x;
    counter->y=y;
    counter->width=width;
    counter->height=height;

    init_draw_indicator(counter);
    draw_numerics(counter,num_length(count),count,-1);
}

/******** local ********/

static void init_draw_indicator(counter)
CountI *counter;
{
    int i;

    if(counter->layout_mode==1) change_layout_info(counter);

    set_color(counter->window,"gray");
    draw_filled_rectangle(counter->window,
        counter->x,
        counter->y,
        counter->width,
        counter->height,
        1,
        NULL,
        pSet);
    clear_numerics(counter,1,counter->length);
}

static int num_length(n)
int n;
{
    int len=1;
    if(n==0) return(len);
    len=0;
    for(;n>0;len++) n=n/10;
    return(len);
}

static void change_layout_info(counter)
CountI *counter;
{
    SWin *win;

    unsigned int margin;
    int line_width;

    margin=counter->width/40;
    line_width=counter->width/40;

    counter->x1=counter->x+margin;
    counter->y1=counter->y+margin;
    counter->w1=(counter->width-(counter->length+1)*margin)/
                (counter->length);
    counter->h1=counter->height-margin*2;

    counter->margin=margin;
    counter->line_width=line_width;

}

static void clear_numerics(counter,place,len)
CountI *counter;
int place;
int len;
{
    int x;
    int y;
    unsigned int w;
    unsigned int h;
    int ww;
    int i;

    y=counter->y1;
    w=counter->w1;
    h=counter->h1;
    ww=(counter->w1)+counter->margin;
    x=(counter->x1)+ww*((counter->length)-place);

    set_color(counter->window,"black");

    for(i=0;i<len;i++){
      draw_filled_rectangle(counter->window,x,y,w,h,1,NULL,pSet);
      x=x-ww;
    }
}

static void draw_numerics(counter,len,number,oldnumber)
CountI *counter;
int len;
int number;
int oldnumber;
{
    int x;
    int y;
    unsigned int w;
    unsigned int h;
    int ww;
    int x0;
    int numeric;
    int max;
    int i;

    int lw;
    SWin *win;

    y=counter->y1+1;  /* +1 -> more inner */
    w=counter->w1-2;  /* -2 -> more inner */
    h=counter->h1-2;  /* -2 -> more inner */
    max=counter->length;
    ww=(counter->w1)+counter->margin;
    x0=counter->x1;

    lw=counter->line_width;
    win=counter->window;

    for(i=0;i<len;i++){
      numeric=number%10;
      if( oldnumber == -1 || numeric != oldnumber%10){
         x=x0+(max-(i+1))*ww+1;  /* +1 -> more inner */
         clear_numerics(counter,i+1,1);
         set_color(win,"white");
         switch(numeric){
            case 0 :
               draw_pos_a(win,x,y,w,h,lw,pSet);
               draw_pos_b(win,x,y,w,h,lw,pSet);
               draw_pos_c(win,x,y,w,h,lw,pSet);
               draw_pos_d(win,x,y,w,h,lw,pSet);
               draw_pos_e(win,x,y,w,h,lw,pSet);
               draw_pos_f(win,x,y,w,h,lw,pSet);
               break;
           case 1 :
               draw_pos_b(win,x,y,w,h,lw,pSet);
               draw_pos_c(win,x,y,w,h,lw,pSet);
               break;
           case 2 :
               draw_pos_a(win,x,y,w,h,lw,pSet);
               draw_pos_b(win,x,y,w,h,lw,pSet);
               draw_pos_g(win,x,y,w,h,lw,pSet);
               draw_pos_e(win,x,y,w,h,lw,pSet);
               draw_pos_d(win,x,y,w,h,lw,pSet);
               break;
           case 3 :
               draw_pos_a(win,x,y,w,h,lw,pSet);
               draw_pos_b(win,x,y,w,h,lw,pSet);
               draw_pos_g(win,x,y,w,h,lw,pSet);
               draw_pos_c(win,x,y,w,h,lw,pSet);
               draw_pos_d(win,x,y,w,h,lw,pSet);
               break;
           case 4 :
               draw_pos_b(win,x,y,w,h,lw,pSet);
               draw_pos_g(win,x,y,w,h,lw,pSet);
               draw_pos_c(win,x,y,w,h,lw,pSet);
               draw_pos_f(win,x,y,w,h,lw,pSet);
               break;
           case 5 :
               draw_pos_a(win,x,y,w,h,lw,pSet);
               draw_pos_f(win,x,y,w,h,lw,pSet);
               draw_pos_g(win,x,y,w,h,lw,pSet);
               draw_pos_c(win,x,y,w,h,lw,pSet);
               draw_pos_d(win,x,y,w,h,lw,pSet);
               break;
           case 6 :
               draw_pos_a(win,x,y,w,h,lw,pSet);
               draw_pos_f(win,x,y,w,h,lw,pSet);
               draw_pos_g(win,x,y,w,h,lw,pSet);
               draw_pos_c(win,x,y,w,h,lw,pSet);
               draw_pos_d(win,x,y,w,h,lw,pSet);
               draw_pos_e(win,x,y,w,h,lw,pSet);
               break;
           case 7 :
               draw_pos_a(win,x,y,w,h,lw,pSet);
               draw_pos_f(win,x,y,w,h,lw,pSet);
               draw_pos_b(win,x,y,w,h,lw,pSet);
               draw_pos_c(win,x,y,w,h,lw,pSet);

               break;
           case 8 :
               draw_pos_a(win,x,y,w,h,lw,pSet);
               draw_pos_b(win,x,y,w,h,lw,pSet);
               draw_pos_c(win,x,y,w,h,lw,pSet);
               draw_pos_d(win,x,y,w,h,lw,pSet);
               draw_pos_e(win,x,y,w,h,lw,pSet);
               draw_pos_f(win,x,y,w,h,lw,pSet);
               draw_pos_g(win,x,y,w,h,lw,pSet);
               break;
           case 9 :
               draw_pos_a(win,x,y,w,h,lw,pSet);
               draw_pos_b(win,x,y,w,h,lw,pSet);
               draw_pos_c(win,x,y,w,h,lw,pSet);
               draw_pos_d(win,x,y,w,h,lw,pSet);
               draw_pos_g(win,x,y,w,h,lw,pSet);
               draw_pos_f(win,x,y,w,h,lw,pSet);
               break;

           default :
               break;
         }
      }
      if(oldnumber != -1) oldnumber=oldnumber/10;
      number=number/10;
      flush(win);
    }
}

static void draw_pos_a(win,x,y,w,h,lw,op)
SWin *win;
int x;
int y;
unsigned int w;
unsigned int h;
int lw;
Optype op;
{
    int x0;
    int y0;
    unsigned int w0;
    unsigned int w2;

    w2=(lw-1)/2;
    x0=x+w2;
    y0=y+w2;
    w0=w-w2*2;

    draw_horizontal(win,x0,y0,w0,lw,op);
}

static void draw_pos_b(win,x,y,w,h,lw,op)
SWin *win;
int x;
int y;
unsigned int w;
unsigned int h;
int lw;
Optype op;
{
    int x0;
    int y0;
    unsigned int h0;
    unsigned int w2;

    w2=(lw-1)/2;
    x0=x+w-w2-1;
    y0=y+w2;
    h0=(h-1)/2-w2+1;

    draw_vertical(win,x0,y0,h0,lw,op);
}

static void draw_pos_c(win,x,y,w,h,lw,op)
SWin *win;
int x;
int y;
unsigned int w;
unsigned int h;
int lw;
Optype op;
{
    int x0;
    int y0;
    unsigned int h0;
    unsigned int w2;

    w2=(lw-1)/2;
    x0=x+w-w2-1;
    y0=y+(h-1)/2;
    h0=(h-1)/2-w2+1;

    draw_vertical(win,x0,y0,h0,lw,op);
}

static void draw_pos_d(win,x,y,w,h,lw,op)
SWin *win;
int x;
int y;
unsigned int w;
unsigned int h;
int lw;
Optype op;
{
    int x0;
    int y0;
    unsigned int w0;
    unsigned int w2;

    w2=(lw-1)/2;
    x0=x+w2;
    y0=y+h-w2-1;
    w0=w-w2*2;

    draw_horizontal(win,x0,y0,w0,lw,op);
}

static void draw_pos_e(win,x,y,w,h,lw,op)
SWin *win;
int x;
int y;
unsigned int w;
unsigned int h;
int lw;
Optype op;
{
    int x0;
    int y0;
    unsigned int h0;
    unsigned int w2;

    w2=(lw-1)/2;
    x0=x+w2;
    y0=y+(h-1)/2;
    h0=(h-1)/2-w2+1;

    draw_vertical(win,x0,y0,h0,lw,op);
}

static void draw_pos_f(win,x,y,w,h,lw,op)
SWin *win;
int x;
int y;
unsigned int w;
unsigned int h;
int lw;
Optype op;
{
    int x0;
    int y0;
    unsigned int h0;
    unsigned int w2;

    w2=(lw-1)/2;
    x0=x+w2;
    y0=y+w2;
    h0=(h-1)/2-w2+1;

    draw_vertical(win,x0,y0,h0,lw,op);
}

static void draw_pos_g(win,x,y,w,h,lw,op)
SWin *win;
int x;
int y;
unsigned int w;
unsigned int h;
int lw;
Optype op;
{
    int x0;
    int y0;
    unsigned int w0;
    unsigned int w2;

    w2=(lw-1)/2;
    x0=x+w2;
    y0=y+(h-1)/2;
    w0=w-w2*2;

    draw_horizontal(win,x0,y0,w0,lw,op);
}


static void draw_horizontal(win,x0,y0,w0,lw,op)
SWin *win;
int x0;
int y0;
unsigned int w0;
int lw;
Optype op;
{
    int x;
    int y;
    unsigned int w;
    unsigned int w2;
    XPoint p[6];

    w2=(lw-1)/2;
    x=x0+1+w2;
    y=y0;
    w=w0-2-w2*2;

    p[0].x=(short) (x);
    p[0].y=(short) (y);
    p[1].x=(short) (x+w2);
    p[1].y=(short) (y-w2);
    p[2].x=(short) (x+w-1-w2);
    p[2].y=(short) (y-w2);
    p[3].x=(short) (x+w-1);
    p[3].y=(short) (y);
    p[4].x=(short) (x+w-1-w2);
    p[4].y=(short) (y+w2);
    p[5].x=(short) (x+w2);
    p[5].y=(short) (y+w2);

/*
printf("(%d,%d) (%d,%d) (%d,%d) (%d,%d) (%d,%d) (%d,%d)\n",
    p[0].x,p[0].y,p[1].x,p[1].y,p[2].x,p[2].y,
    p[3].x,p[3].y,p[4].x,p[4].y,p[5].x,p[5].y);
*/

/*
    XSetLineAttributes(win->display,win->gc,lw,
        LineSolid,CapRound,JoinRound);
    draw_line(win,x0,y,x1,y,pSolid,lw,op);   
*/

    XDrawLines(win->display,win->window,win->gc,
        p,6,CoordModeOrigin);
    XFillPolygon(win->display,win->window,win->gc,
        p,6,Convex,CoordModeOrigin);

}

static void draw_vertical(win,x0,y0,h0,lw,op)
SWin *win;
int x0;
int y0;
unsigned int h0;
int lw;
Optype op;
{
    int x;
    int y;
    unsigned int h;
    unsigned int w2;
    XPoint p[6];

    w2=(lw-1)/2;
    x=x0;
    y=y0+1+w2;
    h=h0-2-w2*2;

    p[0].x=(short) (x);
    p[0].y=(short) (y);
    p[1].x=(short) (x+w2);
    p[1].y=(short) (y+w2);
    p[2].x=(short) (x+w2);
    p[2].y=(short) (y+h-1-w2);
    p[3].x=(short) (x);
    p[3].y=(short) (y+h-1);
    p[4].x=(short) (x-w2);
    p[4].y=(short) (y+h-1-w2);
    p[5].x=(short) (x-w2);
    p[5].y=(short) (y+w2);

/*
    XSetLineAttributes(win->display,win->gc,lw,
        LineSolid,CapRound,JoinRound);
    draw_line(win,x,y0,x,y1,pSolid,lw,op);   
*/

    XDrawLines(win->display,win->window,win->gc,
        p,6,CoordModeOrigin);
    XFillPolygon(win->display,win->window,win->gc,
        p,6,Convex,CoordModeOrigin);

}
