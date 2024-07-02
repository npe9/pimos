#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* Pmeter   1991.1.28 */

#include "pmeter.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static void make_pixmap();
static int num_length();
static void make_pe_name();
static void draw_pe_name();
static void cal_counter();
static void draw_filled_pixmap();
static void draw_filled_area();
static int get_adj_font();
static void draw_string_box();
static void draw_sample();
static void draw_pe();
static void draw_path();
static void draw_ground_path();
static void draw_pe_ground();
static void init_draw_pmeter();
static void cal_pes();
static void cal_window();

extern Pmeter *create_pmeter(pe,matrix_h,matrix_w,x,y,width,height)
int pe;
int matrix_h;
int matrix_w;
int x;
int y;
unsigned int width;
unsigned int height;
{
   Pmeter *pm;
   SWin   *win;
   CountI *count;
   CInfo  cInfo;
   InitInfo wInfo;

   pm=(Pmeter *)malloc( sizeof(Pmeter) );

   pm->pe=pe;
   pm->width=width;
   pm->height=height;
   pm->matrix_h=matrix_h;
   pm->matrix_w=matrix_w;

   cal_window(x,y,width,height,&wInfo);
   win=createSWin(wInfo);
   activateSWin(win);
   pm->window=win;

   cal_pes(pm);
   init_draw_pmeter(pm);

   cal_counter(pm,&cInfo);
   pm->counter=create_count_indicator(pm->window,cInfo);

   make_pixmap(pm);

   return(pm);
}



extern void draw_pmeter(pm,data)
Pmeter *pm;
unsigned short *data;
{
   CInfo cInfo;
   unsigned int width;
   unsigned int height;

   unsigned short idl1;
   unsigned short idl2;
   unsigned short run1;
   unsigned short run2;
   unsigned long idl;
   unsigned long run;
   int percent;
   int i;
   SWin *win;

   if((get_reshape(pm->window,&width,&height)==1)&& (pm->width !=width || pm->height != height)) {
      pm->width=width;
      pm->height=height;
      cal_counter(pm,&cInfo);
      clear_area(pm->window,0,0,width,height);
      cal_pes(pm);
      init_draw_pmeter(pm);
      reshape_counter(pm->counter,
         cInfo.x,cInfo.y,cInfo.width,cInfo.height);
      make_pixmap(pm);      
   }

   countup(pm->counter);

   for(i=0;i<pm->pe;i++){
      idl1= *(data+i*5);
      idl2= *(data+i*5 + 1);
      run1= *(data+i*5 + 2);
      run2= *(data+i*5 + 3);
      idl=idl1*65535+idl2;
      run=run1*65535+run2;
      if(idl==0 && run ==0) {
         percent=0;
      } else {
         percent=(run*100)/(idl+run);
      }

      win=pm->window;
      if(percent>=100) percent=99;
      XCopyArea(win->display,pm->pixmap[9-percent/10],win->window,win->gc,
         0,0,pm->pe_width,pm->pe_height,
         pm->x0 + (pm->pe_interval+pm->pe_width) * (i % pm->matrix_w),
         pm->y0 - (pm->pe_interval+pm->pe_height) * (i / pm->matrix_w));

   }
}

extern void kill_pmeter(pm)
Pmeter *pm;
{
   killSWin(pm->window);
   free(pm);
}

extern void set_pm_title(pm,title)
Pmeter *pm;
char *title;
{
  SWin *win;

  win = pm->window;
  set_title(win,title);
}
  
/******** local ********/

static void cal_window(x,y,width,height,wInfo)
int x;
int y;
unsigned int width;
unsigned int height;
InitInfo *wInfo;
{
   resetWinit(wInfo);
   wInfo->x=x;
   wInfo->y=y;
   wInfo->width=width;
   wInfo->height=height;
   wInfo->title=" ";
}

static void cal_pes(pm)
Pmeter *pm;
{
   unsigned int width;
   unsigned int height;
   int pes_len;
   unsigned int margin;
   unsigned int p_margin;
   unsigned int bottom_margin;
   unsigned int pw;
   unsigned int ph;
   unsigned int box_margin;

   width=pm->width*3/4;
   height=pm->height;
/*
   if(pm->matrix_w > pm->matrix_h) {
      pes_len=pm->matrix_w;
   } else {
      pes_len=pm->matrix_h;
   }
*/
   margin=width/50;
/*
   bottom_margin=((height-margin*2)/(pes_len+1))/3+margin;
   pw=(width-margin*2)/pes_len;
   ph=(height-margin-bottom_margin)/pes_len;
*/

   bottom_margin=((height-margin*2)/(pm->matrix_h+1))/3+margin;
   pw=(width-margin*2)/(pm->matrix_w);
   ph=(height-margin-bottom_margin)/(pm->matrix_h);


   if(pw>ph) {
      p_margin=ph/6; /* 1992.1.28 */
   } else {
      p_margin=pw/6; /* 1992.1.28 */
   }

   box_margin=p_margin/4;

   pm->pe_box_margin=box_margin;
   pm->pe_width=pw-p_margin*2-box_margin*2;
   pm->pe_height=ph-p_margin*2-box_margin*2;
   pm->pe_interval=p_margin*2+box_margin*2;
   pm->x0=margin+p_margin+box_margin;
   pm->y0=height-bottom_margin-ph+p_margin+box_margin;

}

static void init_draw_pmeter(pm)
Pmeter *pm;
{
   SWin *win;
   unsigned int width;
   unsigned int height;
   
   char check[32];
   char mesh[8];

   check[0]=0x77;
   check[1]=0x77;
   check[2]=0xbb;
   check[3]=0xbb;
   check[4]=0xee;
   check[5]=0xee;
   check[6]=0xdd;
   check[7]=0xdd;
   check[8]=0x77;
   check[9]=0x77;
   check[10]=0xbb;
   check[11]=0xbb;
   check[12]=0xee;
   check[13]=0xee;
   check[14]=0xdd;
   check[15]=0xdd;
   check[16]=0x77;
   check[17]=0x77;
   check[18]=0xbb;
   check[19]=0xbb;
   check[20]=0xee;
   check[21]=0xee;
   check[22]=0xdd;
   check[23]=0xdd;
   check[24]=0x77;
   check[25]=0x77;
   check[26]=0xbb;
   check[27]=0xbb;
   check[28]=0xee;
   check[29]=0xee;
   check[30]=0xdd;
   check[31]=0xdd;

   mesh[0]=0x00;
   mesh[1]=0x7e;
   mesh[2]=0x7e;
   mesh[3]=0x7e;
   mesh[4]=0x7e;
   mesh[5]=0x7e;
   mesh[6]=0x7e;
   mesh[7]=0x00;

   win=pm->window;
   width=pm->width;
   height=pm->height;

   set_color(win,"DarkSlateGray");
   draw_filled_rectangle(win,0,0,width,height,2,NULL,pSet);
   draw_filled_area(win,width*3/4,0,width/4,height,check);
   draw_pe_ground(pm,check,16,16);
   draw_ground_path(pm,check,16,16);
   draw_path(pm,mesh,8,8);
   draw_pe(pm);
   draw_pe_name(pm);
   draw_sample(pm);

}

static void draw_pe_ground(pm,pt,pw,ph)
Pmeter *pm;
char *pt;
int pw;
int ph;
{
   SWin *win;
   unsigned int dh;
   unsigned int dw;
   int x;
   int y;
   unsigned int w;
   unsigned int h;
   int matrix_w;
   int matrix_h;
   int i;
   int j;
   int x0;
   Pixmap pixmap;

   win=pm->window;
   dw=pm->pe_interval + pm->pe_width;
   dh=pm->pe_interval + pm->pe_height;
   x=pm->x0 - pm->pe_box_margin + pm->pe_interval/8;
   y=pm->y0 - pm->pe_box_margin + pm->pe_interval/8;
   w=pm->pe_width + pm->pe_box_margin*2;
   h=pm->pe_height + pm->pe_box_margin*2;
   matrix_w=pm->matrix_w;
   matrix_h=pm->matrix_h;
   x0=x;

   pixmap = XCreatePixmap(win->display,win->window,w,h,
            DefaultDepth(win->display,win->screen_number));

   draw_filled_pixmap(win,&pixmap,0,0,w,h,pt,pw,ph);

   for(i=0;i<matrix_h;i++){
      for(j=0;j<matrix_w;j++){
         XCopyArea(win->display,pixmap,win->window,win->gc,
                   0, 0, w, h, x, y);
         x=x+dw;
      }
      y=y-dh;
      x=x0;
   }
   flush(win);
   XFreePixmap(win->display,pixmap);
}

static void draw_ground_path(pm,pt,pw,ph)
Pmeter *pm;
char *pt;
int pw;
int ph;
{
   SWin *win;
   unsigned int dh;
   unsigned int dw;
   int x;
   int y;
   unsigned int w;
   unsigned int h;
   int matrix_w;
   int matrix_h;
   int i;
   int j;
   int x0;
   Pixmap pixmap;

   /* vertical */

   win=pm->window;
   dw=pm->pe_interval + pm->pe_width;
   dh=pm->pe_interval + pm->pe_height;
   x=pm->x0 - pm->pe_box_margin + pm->pe_interval/8
            + pm->pe_width/2 - pm->pe_interval/8 + pm->pe_box_margin;
   y=pm->y0 - pm->pe_box_margin + pm->pe_interval/8 
            - pm->pe_interval + pm->pe_box_margin*2;
   w=pm->pe_interval/4;
   h=pm->pe_interval - pm->pe_box_margin*2;
   matrix_w=pm->matrix_w;
   matrix_h=pm->matrix_h;
   x0=x;

   pixmap = XCreatePixmap(win->display,win->window,w,h,
            DefaultDepth(win->display,win->screen_number));

   draw_filled_pixmap(win,&pixmap,0,0,w,h,pt,pw,ph);

   for(i=1;i<matrix_h;i++){
      for(j=0;j<matrix_w;j++){
         XCopyArea(win->display,pixmap,win->window,win->gc,
                   0, 0, w, h, x, y);
         x=x+dw;
      }
      y=y-dh;
      x=x0;
   }
   flush(win);

   /* horizontal */

   x=pm->x0 - pm->pe_box_margin + pm->pe_interval/8
            + pm->pe_width + pm->pe_box_margin*2;
   y=pm->y0 - pm->pe_box_margin + pm->pe_interval/8
            + pm->pe_height/2 - pm->pe_interval/8 + pm->pe_box_margin;
   w=pm->pe_interval - pm->pe_box_margin*2;
   h=pm->pe_interval/4;
   x0=x;

   pixmap = XCreatePixmap(win->display,win->window,w,h,
            DefaultDepth(win->display,win->screen_number));

   draw_filled_pixmap(win,&pixmap,0,0,w,h,pt,pw,ph);

   for(i=0;i<matrix_h;i++){
      for(j=1;j<matrix_w;j++){
         XCopyArea(win->display,pixmap,win->window,win->gc,
                   0, 0, w, h, x, y);
         x=x+dw;
      }
      y=y-dh;
      x=x0;
   }
   flush(win);

   XFreePixmap(win->display,pixmap);
}

static void draw_path(pm,pt,pw,ph)
Pmeter *pm;
char *pt;
int pw;
int ph;
{
   SWin *win;
   unsigned int dh;
   unsigned int dw;
   int x;
   int y;
   unsigned int w;
   unsigned int h;
   int matrix_w;
   int matrix_h;
   int i;
   int j;
   int x0;
   unsigned int adj;
   Pixmap pixmap;

   /* vertical */

   win=pm->window;
   dw=pm->pe_interval + pm->pe_width;
   dh=pm->pe_interval + pm->pe_height;
   x=pm->x0 - pm->pe_box_margin + pm->pe_interval/4
            + pm->pe_width/2 - pm->pe_interval/8 + pm->pe_box_margin;
   y=pm->y0 - pm->pe_box_margin + pm->pe_interval/4 
            - pm->pe_interval + pm->pe_box_margin*2;
   w=pm->pe_interval/4;
   h=pm->pe_interval - pm->pe_box_margin*2;
   matrix_w=pm->matrix_w;
   matrix_h=pm->matrix_h;

   /* ADD */
   adj=pm->pe_interval/4;
   x=x-adj;
   y=y-adj;

   x0=x;

   pixmap = XCreatePixmap(win->display,win->window,w,h,
            DefaultDepth(win->display,win->screen_number));

   draw_filled_pixmap(win,&pixmap,0,0,w,h,pt,pw,ph);

   for(i=1;i<matrix_h;i++){
      for(j=0;j<matrix_w;j++){
         XCopyArea(win->display,pixmap,win->window,win->gc,
                   0, 0, w, h, x, y);
         x=x+dw;
      }
      y=y-dh;
      x=x0;
   }
   flush(win);

   /* horizontal */

   x=pm->x0 - pm->pe_box_margin + pm->pe_interval/4
            + pm->pe_width + pm->pe_box_margin*2;
   y=pm->y0 - pm->pe_box_margin + pm->pe_interval/4
            + pm->pe_height/2 - pm->pe_interval/8 + pm->pe_box_margin;
   w=pm->pe_interval - pm->pe_box_margin*2;
   h=pm->pe_interval/4;

   /* ADD */
   adj=pm->pe_interval/4;
   x=x-adj;
   y=y-adj;

   x0=x;

   pixmap = XCreatePixmap(win->display,win->window,w,h,
            DefaultDepth(win->display,win->screen_number));

   draw_filled_pixmap(win,&pixmap,0,0,w,h,pt,pw,ph);

   for(i=0;i<matrix_h;i++){
      for(j=1;j<matrix_w;j++){
         XCopyArea(win->display,pixmap,win->window,win->gc,
                   0, 0, w, h, x, y);
         x=x+dw;
      }
      y=y-dh;
      x=x0;
   }
   flush(win);

   XFreePixmap(win->display,pixmap);

}

static void draw_pe(pm)
Pmeter *pm;
{
   SWin *win;
   unsigned int dh;
   unsigned int dw;
   int x;
   int y;
   unsigned int w;
   unsigned int h;
   int matrix_w;
   int matrix_h;
   int i;
   int j;
   int x0;
   unsigned int adj;
   Pixmap pixmap;

   win=pm->window;
   dw=pm->pe_interval + pm->pe_width;
   dh=pm->pe_interval + pm->pe_height;
   x=pm->x0 - pm->pe_box_margin + pm->pe_interval/4;
   y=pm->y0 - pm->pe_box_margin + pm->pe_interval/4;
   w=pm->pe_width + pm->pe_box_margin*2;
   h=pm->pe_height + pm->pe_box_margin*2;
   matrix_w=pm->matrix_w;
   matrix_h=pm->matrix_h;

   /* ADD */
   adj=pm->pe_interval/4;
   x=x-adj;
   y=y-adj;

   x0=x;

   set_color(win,"black");

   pixmap = XCreatePixmap(win->display,win->window,w,h,
            DefaultDepth(win->display,win->screen_number));

   draw_filled_pixmap(win,&pixmap,0,0,w,h,0,0,0);

   for(i=0;i<matrix_h;i++){
      for(j=0;j<matrix_w;j++){
         XCopyArea(win->display,pixmap,win->window,win->gc,
                   0, 0, w, h, x, y);
         x=x+dw;
      }
      y=y-dh;
      x=x0;
   }
   flush(win);

   XFreePixmap(win->display,pixmap);

}

static void draw_sample(pm)
Pmeter *pm;
{
   char *c[10];
   char *s[6];
   SWin *win;

   int x;
   int y;
   int i;
   unsigned int width;
   unsigned int height;
   unsigned int w;
   unsigned int h;

   win=pm->window;
   width=pm->width;
   height=pm->height;

   x=width*3/4+width/20;
   y=height/5;
   w=width/15;
   h=height/15;

   c[0]=Pcolor0;
   c[1]=Pcolor1;
   c[2]=Pcolor2;
   c[3]=Pcolor3;
   c[4]=Pcolor4;
   c[5]=Pcolor5;
   c[6]=Pcolor6;
   c[7]=Pcolor7;
   c[8]=Pcolor8;
   c[9]=Pcolor9;

   s[0]="  0%";
   s[1]=" 20%";
   s[2]=" 40%";
   s[3]=" 60%";
   s[4]=" 80%";
   s[5]="100%";

   for(i=0;i<10;i++){
      set_color(win,c[i]);
      draw_filled_rectangle(win,x,y,w,h,1,NULL,pSet);
      y=y+h;
   }

   y=y-h/2;
   for(i=0;i<6;i++){
      draw_string_box(win,x+w+1+w/10,y+h/4,w,h/2,s[i]);
      y=y-h*2;
   }

}


static void draw_string_box(win,x,y,w,h,s)
SWin *win;
int x;
int y;
unsigned int w;
unsigned int h;
char *s;
{
   char font[150];
   unsigned int fw;
   unsigned int fh;
   unsigned int charH;
   unsigned int charW;

   int flag;

   flag=get_adj_font(w/strlen(s),h,&fw,&fh,font);
   if(flag ==1 ){
      set_color(win,"black");
      draw_filled_rectangle(win,x,y,w,h,1,NULL,pSet);
      set_color(win,"white");
      set_font(win,font);/* hamanaka 2/6 */
      get_font_size(win,&charW,&charH);/* hamanaka 2/6 */
/*    draw_string(win,x+(w-fw*4)/2,y+(h-fh)/2+fh,s,font,pSet);*/
      draw_string(win,x+(w-fw*4)/2,y+(h-fh)/2+fh-charH,s,font,pSet);/* hamanaka 2/6 */
   }

}

static int get_adj_font(w,h,fw,fh,font)
unsigned int w;
unsigned int h;
unsigned int *fw;
unsigned int *fh;
char *font;
{
   if(h<8 || w<6) return(0);
   if(h<11 || w<8){
      strcpy(font,"-bitstream-charter-bold-r-normal--8-80-75-75-p-50-iso8859-1");
      *fw=6; /* fw=8 */
      *fh=8; /* fh=11 */
      return(1);
   }
   if(h<14 || w<10){
      strcpy(font,"-bitstream-charter-bold-r-normal--11-80-100-100-p-69-iso8859-1");
      *fw=8; /* fw=11 */
      *fh=11; /* fh=13 */
      return(1);
   }
   if(h<19 || w<13){
      strcpy(font,"-bitstream-charter-bold-r-normal--14-100-100-100-p-88-iso8859-1");
      *fw=10; /* fw=14 */
      *fh=14; /* fh=17 */
      return(1);
   }
   if(h<25 || w<18){
      strcpy(font,"-bitstream-charter-bold-r-normal--19-140-100-100-p-119-iso8859-1");
      *fw=13; /* fw=20 */
      *fh=19; /* fh=23 */
      return(1);
   }
   if(h<33 || w<24){
      strcpy(font,"-bitstream-charter-bold-r-normal--25-180-100-100-p-157-iso8859-1");
      *fw=18; /* fw=25 */
      *fh=25; /* fh=30 */
      return(1);
   }

      strcpy(font,"-bitstream-charter-bold-r-normal--33-240-100-100-p-206-iso8859-1");
   *fw=24; /* fw=34 */
   *fh=33; /* fh=40 */
   return(1);
}

static void draw_filled_area(win,x,y,width,height,pattern)
SWin *win;
int x;
int y;
unsigned int width;
unsigned int height;
char *pattern;
{
   set_color(win,"white");
   draw_filled_rectangle(win,x,y,width,height,1,NULL,pSet);
   set_color(win,"black");
   draw_filled_rectangle(win,x,y,width,height,1,pattern,pSet);
}

static void draw_filled_pixmap(win,pixmap,x,y,width,height,pattern,pw,ph)
SWin *win;
Pixmap *pixmap;
int x;
int y;
unsigned int width;
unsigned int height;
char *pattern;
int pw;
int ph;
{
   Pixmap pixmap0;

   if(pattern!=0){
      set_color(win,"white");
      XFillRectangle(win->display,*pixmap,win->gc,0,0,width,height);
      set_color(win,"black");
      pixmap0 = XCreateBitmapFromData(win->display,win->window,pattern,pw,ph);
      XSetStipple(win->display,win->gc,pixmap0);
      XSetFillStyle(win->display,win->gc,FillStippled);
      XFillRectangle(win->display,*pixmap,win->gc,0,0,width,height);
      XSetFillStyle(win->display,win->gc,FillSolid);
      XFreePixmap(win->display,pixmap0);
   }else{
      XFillRectangle(win->display,*pixmap,win->gc,0,0,width,height);
   }
}

static void cal_counter(pm,cInfo)
Pmeter *pm;
CInfo *cInfo;
{
  cInfo->x=(pm->width*3)/4+20;
  cInfo->y=20;
  cInfo->width=(pm->width)/4-20*2;
  cInfo->height=pm->height/12;
  cInfo->length=8;
  cInfo->layout_mode=1;
}


static void draw_pe_name(pm)
Pmeter *pm;
{
   SWin *win;
   unsigned int dh;
   unsigned int dw;
   int x;
   int y;
   unsigned int w;
   unsigned int h;
   int matrix_w;
   int matrix_h;
   int i;
   int j;
   int x0;
   unsigned int adj;
   int num;
   char pe_name[10];

   win=pm->window;

   dw=pm->pe_interval + pm->pe_width;
   dh=pm->pe_interval + pm->pe_height;
   x=pm->x0 - pm->pe_box_margin + pm->pe_interval/4;
   y=pm->y0 - pm->pe_box_margin + pm->pe_interval/4;
   w=pm->pe_width + pm->pe_box_margin*2;
   h=pm->pe_height + pm->pe_box_margin*2;
   matrix_w=pm->matrix_w;
   matrix_h=pm->matrix_h;

   /* ADD */
   adj=pm->pe_interval/4;
   x=x-adj;
   y=y-adj+h+adj/4;

   x=x+w/3;  /* Attention !!! */
   h=h/4;    /* Attention !!! */
   w=w*2/3;  /* Attention !!! */
   x0=x;

   num=0;
   set_color(win,"black");
   for(i=0;i<matrix_h;i++){
      for(j=0;j<matrix_w;j++){
         make_pe_name(num,pe_name);
         draw_string_box(win,x,y,w,h,pe_name);
         num++;
         x=x+dw;
      }
      y=y-dh;
      x=x0;
   }
   flush(win);
   
}

static void make_pe_name(n,pn)
int n;
char *pn;
{
   int i;
   int len;

   *(pn+0)='P';
   *(pn+1)='E';
   *(pn+2)='0';
   *(pn+3)='0';

   len=num_length(n);
   for(i=0;i<len;i++) {
      *(pn+4-i)='0'+n%10;
      n=n/10;
   }

   *(pn+5)=NULL;

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

static void make_pixmap(pm)
Pmeter *pm;
{
    Pixmap pixmap;
    SWin *win;
    int i;
    unsigned int w;
    unsigned int h;
    char *c[10];

    win=pm->window;

    c[0]=Pcolor0;
    c[1]=Pcolor1;
    c[2]=Pcolor2;
    c[3]=Pcolor3;
    c[4]=Pcolor4;
    c[5]=Pcolor5;
    c[6]=Pcolor6;
    c[7]=Pcolor7;
    c[8]=Pcolor8;
    c[9]=Pcolor9;

   w=pm->pe_width;
   h=pm->pe_height;

   for(i=0;i<10;i++){
      pixmap = XCreatePixmap(win->display,win->window,w,h,
            DefaultDepth(win->display,win->screen_number));
      set_color(win,c[i]);
      XFillRectangle(win->display,pixmap,win->gc,0,0,w,h);
      pm->pixmap[i]=pixmap;
   }
}


