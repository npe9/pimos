#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/***************************************************/
/*  1992.4.27                                      */
/*  PIM/p runtime-monitor 20 colors                */
/*  version 4.27                                   */
/***************************************************/

#include <math.h>
#include "prmon.h"

#define   ULONG_MAX        4294967295 /* ulong MAX for psi3 */
#define   FrameWidth       10
#define   MarginTop        (FrameWidth+10)
#define   MarginLeft       (FrameWidth+20)
#define   MarginRight      (FrameWidth+40)
#define   MarginDown       (FrameWidth)
#define   SampleMarginDown (FrameWidth+3)
#define   SampleMarginTop  (FrameWidth+3)
#define   Interval         35
#define   IntervalW        15
#define   IntervalH        15
#define   SampleMinInterval 5
#define   SampleIntervalRange       20
#define   SampleRange      20
#define   SampleMinW       1
#define   SampleMinH       1
#define   r14Height        14
#define   colors           20
#define   BorderHeight     300
#define   BVerBackColor         (char *)"DarkSlateGray"
#define   BVerShadowColor       (char *)"black"
#define   BVerStringColor       (char *)"white"
#define   BVerStringShadowColor (char *)"black"
#define   BVerPositivColor      (char *)"LightSlateGray"
#define   WVerBackColor         (char *)"white"
#define   WVerShadowColor       (char *)"white"
#define   WVerStringColor       (char *)"black"
#define   WVerStringShadowColor (char *)"white"

#define   FrameShadowColor       (char *)"black"
#define   FramePositivColor       (char *)"lavender"

static void killPixM();
static void draw_frame();
static void reshape_window();
static void copy_pixmap();
static void set_palette();
static void draw_percent();
static void init_draw();
static void get_init_data();
static void draw_cluster();
static void scroll_graph();
static void draw_sample();
static char *get_number_string();
static void copy_sample();
static void draw_data();
static void draw_triangle();
static void createPixmap();
static unsigned int cal_sample_height();
static void cal_sample_position();
static void cal_sample_size();
static void cal_window_min_size();

/********** resetPinit **********/
void resetPRinit(rinit)
PRInit *rinit;
{
    rinit -> x = 0;
    rinit -> y = 0;
    rinit -> width = 690;
    rinit -> height = 250;
    rinit -> scroll = 4;
    rinit -> cycle = 10;
    rinit -> type = white;
    rinit -> uneven = convex;
    rinit -> display = (int)NULL;
    rinit -> title = "PIM/p Runtime Monitor";
}

/********** create_rmonitor **********/
PRmon *create_prmon(Cluster,ClusterH,ClusterW,PE,rinit)
unsigned int PE;
unsigned int Cluster;
unsigned int ClusterH;
unsigned int ClusterW;
PRInit rinit;
{
    InitInfo wInfo;
    PRmon     *rmon;
    SWin      *win;
    resetWinit(&wInfo);
    rmon = (PRmon *)malloc(sizeof(PRmon));
    get_init_data(PE,Cluster,ClusterH,ClusterW,&rinit,&wInfo,rmon);
    set_palette(rmon);
    win = createSWin(wInfo);
    rmon->window = win;
    createPixmap(rmon);
    init_draw(rmon);
    return(rmon);
}

/********** activate_rmonitor **********/
void activate_prmon(rmon)
PRmon *rmon;
{
  activateSWin(rmon->window);
}

/********** deactivate_rmonitor **********/
void deactivate_prmon(rmon)
PRmon *rmon;
{
  deactivateSWin(rmon->window);
}

/********** kill_rmonitor **********/
void kill_prmon(rmon)
PRmon *rmon;
{
  killPixM(rmon);
  killSWin(rmon->window);
  free(rmon);
}
/********** draw_rmonitor **********/
void draw_prmon(rmon,data)
PRmon *rmon;
unsigned short *data;
{
    int count;
    reshape_window(rmon);
    scroll_graph(rmon);
    count = rmon->count;
    draw_data(rmon,data);
    rmon->count = count + 1;

}

/*************************/
/********** sub **********/
/*************************/

/********** reshape_window **********/
static void reshape_window(rmon)
PRmon *rmon;
{
  int reshape=NO;
  unsigned int width;
  unsigned int height;
  unsigned int pe;
  unsigned int pe0;
  unsigned int GraphWidth;
  unsigned int GraphHeight;
  unsigned int sw;
  unsigned int sh;

  reshape = get_reshape(rmon->window,&width,&height);
  
  if (reshape == YES && (width != rmon->width || height != rmon->height)){

    killPixM(rmon);
    rmon -> count = 0;
    rmon -> width = width;
    rmon -> height = height;
    cal_sample_size(rmon,&sw,&sh);
    rmon->sw = sw;
    rmon->sh = sh;

    createPixmap(rmon);
    clear_area(rmon->window,0,0,width,height);
    draw_frame(rmon);
    draw_cluster(rmon);
    draw_percent(rmon);
    draw_sample(rmon);
  }
}
/********** killPixM **********/
static void killPixM(rmon)
PRmon *rmon;
{
  int i;

  for(i=0;i<=colors*2;i++)
    {
      XFreePixmap(rmon->window->display,rmon->pixmap[i]);
      /****** !!! ********/
    }
}

/********** createPixmap **********/
static void createPixmap(rmon)
PRmon *rmon;
{
  Pixmap pixmap1;
  Pixmap pixmap2;
  int i;
  float sh=0;
  float sw=0;
  
  SWin *win;
  double twidth=0;
  double theight=0;
  double gcWidth;
  double gcHeight;
  unsigned int gcYTop;
  unsigned int gcYDown;
  unsigned int F;
  win = rmon->window;
  
  for(i=0;i<colors;i++){
    pixmap1 = XCreatePixmap(win->display, win->window, rmon->sw, rmon->sh,
			   DefaultDepth(win->display,win->screen_number));
    pixmap2 = XCreatePixmap(win->display, win->window, rmon->sw, rmon->sh,
			   DefaultDepth(win->display,win->screen_number));
    
    set_color(rmon->window,*(rmon->Color+(colors-1-i)));
    XSetLineAttributes(win->display,win->gc,1,LineSolid,CapButt,JoinMiter);
    XSetFunction(win->display,win->gc,GXcopy);
    XFillRectangle(win->display,pixmap1,win->gc,0,0,rmon->sw,rmon->sh);
    XFillRectangle(win->display,pixmap2,win->gc,0,0,rmon->sw,rmon->sh);
/*
    set_color(rmon->window,"black");
    XDrawRectangle(win->display,pixmap1,win->gc,0,0,rmon->sw-1,rmon->sh-1);
    XDrawRectangle(win->display,pixmap2,win->gc,0,0,rmon->sw-1,rmon->sh-1);
*/
    if((rmon->sw) > (rmon->sh)){
      sh = (rmon->sh);
      twidth = sh;
    }else{
      sw= (rmon->sw);
      twidth = sw;
    }
    theight = twidth/2*1.7320508;
    if(rmon->sw <= 3 || rmon->sh <=3){
      if(rmon->sw < rmon->sh){
	gcWidth = rmon->sw;
      }else{
	gcWidth = rmon->sh;
      }
      /*gc mark */
      set_color(rmon->window,"black");
      XSetFunction(win->display,win->gc,GXcopy);
      if(rmon->sw < rmon->sh || ((rmon->sh == rmon->sw) && rmon->sh != 1) ){
	gcWidth = rmon->sw;
	gcHeight = rmon->sh / 2;
	if(gcHeight > 10)gcHeight=10; /* gc height */
	gcYTop = (int)(rmon->sh /2 - gcHeight /2);
	gcYDown = (int)(rmon->sh/2 + gcHeight /2);
	XSetLineAttributes(win->display,win->gc,(unsigned int)gcWidth,
			   LineSolid,CapButt,JoinMiter);
	XDrawLine(win->display,pixmap2,win->gc,
		  rmon->sw/2,gcYTop+1,rmon->sw/2,gcYDown+1);
      }else{
	gcWidth = rmon->sh;
	gcHeight = rmon->sh;
	XSetLineAttributes(win->display,win->gc,(unsigned int)gcWidth,
			   LineSolid,CapButt,JoinMiter);
	XDrawLine(win->display,pixmap2,win->gc,
		  rmon->sw/2,0,rmon->sw/2,rmon->sh);
      }
    }else{
      draw_triangle(rmon,pixmap2,rmon->sw/2,
		    rmon->sh/2-(int)(theight/(float)2),
		    (unsigned int)twidth,(unsigned int)theight);
    }
    rmon->pixmap[i] = pixmap1;
    rmon->pixmap[i+colors] = pixmap2;
  }
}

/********** draw_triangle **********/
static void draw_triangle(rmon,pixmap,x,y,width,height)
PRmon *rmon;
Pixmap pixmap;
int x;
int y;
unsigned int width;
unsigned int height;
{
    SWin *win;
    XPoint p[8];
    int x1;
    int x2;
    int x3;
    int x4;
    int y1;
    int y2;
    int y3;
    int y4;

    win = rmon->window;
    x1 = x;
    x2 = width/2;
    x3 = -width;
    x4 = width/2;
    y1 = y;
    y2 = height;
    y3 = 0;
    y4 = -height;

    p[0].x = x1;
    p[1].x = x2;
    p[2].x = x3;
    p[3].x = x4;

    p[0].y = y1;
    p[1].y = y2;
    p[2].y = y3;
    p[3].y = y4;
    XSetFunction(win->display,win->gc,GXcopy);
    XSetLineAttributes(win->display,win->gc,1,LineSolid,CapButt,JoinMiter);
    set_color(rmon->window,"white");
    XFillPolygon(win->display,pixmap,win->gc,p,3,Convex,CoordModePrevious);
    set_color(rmon->window,"black");
    XDrawLines(win->display,pixmap,win->gc,p,4,CoordModePrevious);
}

/********** scroll **********/
static void scroll_graph(rmon)
PRmon *rmon;
{
    int x1;
    int x2;
    int x3;
    int y;
    int i;
    int j;
    unsigned int w;
    unsigned int h;
    unsigned int w1;
    unsigned int h1;
    unsigned int scroll;
    unsigned int GraphWidth;
    unsigned int GraphHeight;
    unsigned int charH;
    unsigned int charW;
    int F;

    scroll = rmon->cycle / rmon->scroll;
    if(scroll < 1)scroll= 1;
    if(rmon->count == rmon->cycle){
      F = get_font_flag(rmon);
      get_font_size(rmon->window,&charW,&charH);
      GraphWidth = rmon->cycle*rmon->sw;
      for(j=rmon->ClusterH-1;j>=0;j--){
	for(i=0;i<rmon->ClusterW;i++){
	  x1 = MarginLeft + scroll * rmon->sw + GraphWidth*i + IntervalW * i;
	  y = MarginTop + (rmon->sh * rmon->pe +IntervalH+ charH*F) * j;
	  w = (rmon->cycle - scroll)*rmon->sw;
	  h = rmon -> pe * rmon ->sh;
	  x2 = MarginLeft+ GraphWidth*i + IntervalW * i;
	  x3 = MarginLeft + w + GraphWidth*i + IntervalW * i;
	  w1 = scroll * rmon -> sw;
	  h1 = rmon -> pe * rmon-> sh;

	  transfer(rmon->window,x1,y,w,h,x2,y);
	  
	  XCopyArea(rmon->window->display,rmon->pixmap[colors*2],
		    rmon->window->window,rmon->window->gc,0,0,w1,h1,x3,y);
/*	  clear_area(rmon->window,x3,y,w1,h1);*/
	}
      }
      rmon -> count = rmon -> cycle - scroll;
    }
}
/********** draw_data **********/
static void draw_data(rmon,data)
PRmon *rmon;
unsigned short *data;
{
    int counter = 0;
    int pos = 0;
    int i;
    int j;
    int p;
    unsigned short a1;
    unsigned short a2;
    unsigned short na1;
    unsigned short na2;
    unsigned short gc;
    unsigned long a;
    unsigned long na;
    unsigned long total;
    for(i=0;i<rmon->pe * rmon->Cluster;i++){
	pos = counter * 5;
	a1 = *(data+pos);
	a2 = *(data+pos+1);
	na1 =*(data+pos+2);
	na2 =*(data+pos+3);
	gc = *(data+pos+4);
	a = a1 * 65536 + a2;
	na = na1 * 65536 + na2;
	if (ULONG_MAX - a <= na){
	    total = ULONG_MAX;
	}else{
	    total = a + na;
	}

	if (total == 0){
	    p = 0;
	}else{
	  if(total/ colors == 0){
	    p = na*colors / total;
	  }else{
	    p = na / (total/ colors);
	  }
	}

	if (p>colors-1)p=colors-1;
	copy_sample(rmon,p,counter,gc);
	counter++;
    }
    flush(rmon->window);
}

/********** copy_sample **********/
static void copy_sample(rmon,p,counter,gc)
PRmon *rmon;
int p;
int counter;
unsigned short gc;
{
    int x1;
    int y1;
    int p0;
    int num = 0;
    unsigned int count=0;
    unsigned int ClusterX=0;
    unsigned int ClusterY=0;
    unsigned int GraphWidth;
    unsigned int GraphHeight;
    int F;
    unsigned int charH=14;
    unsigned int charW;

    GraphWidth = rmon->sw * rmon -> cycle;
    GraphHeight = rmon->sh*rmon->pe;
    F = get_font_flag(rmon);
    if(counter >0 && rmon->pe >0) {
      count = counter % rmon->pe;
      num = counter / rmon->pe;
      if(num >0){
	ClusterX = num % rmon->ClusterW;
      }
    }
    ClusterY = rmon->ClusterH -num / rmon->ClusterW -1;

    x1 = MarginLeft + rmon->count * rmon->sw 
       + GraphWidth * ClusterX + IntervalW * ClusterX;
    y1 = MarginTop + (GraphHeight + IntervalH + charH*F) * ClusterY 
      + rmon -> sh * (rmon->pe -count -1);
    if(gc==1){
      p0 = p+colors;
    }else{
      p0 =  p;
    }
    copy_pixmap(rmon,p0,x1,y1);
}

/********** copy_pixmap ********/
static void copy_pixmap(rmon,p,x,y)
PRmon *rmon;
int p;
int x;
int y;
{
  SWin *win;
  win = rmon->window;
  XCopyArea(win->display,rmon->pixmap[p],win->window,win->gc,0,0,rmon->sw,rmon->sh,x,y);
  
}

/********** get_init_data **********/
static void get_init_data(pe,Cluster,ClusterH,ClusterW,rInit,wInfo,rmon)
unsigned int pe;
unsigned int Cluster;
unsigned int ClusterH;
unsigned int ClusterW;
PRInit *rInit;
InitInfo *wInfo;
PRmon *rmon;
{
    unsigned int GraphHeight;
    unsigned int GraphWidth;
    unsigned int pe0;
    unsigned int sw;
    unsigned int sh;
    int min_width;
    int min_height;
    int min_height1;
    int min_height2;

    rmon -> ClusterH = ClusterH;
    rmon -> ClusterW = ClusterW;
    rmon -> pe = pe;
    rmon -> cycle = rInit -> cycle;
    rmon -> display = rInit -> display;
    rmon -> type = rInit -> type;
    rmon -> uneven = rInit -> uneven;
    cal_window_min_size(rmon,&min_width,&min_height);
    wInfo -> min_width = min_width;
    wInfo -> min_height = min_height;

    if(rInit->width > min_width){
      wInfo -> width = rInit->width;
    }else{
      wInfo -> width = min_width;
    }
    if(rInit -> height > min_height){
      wInfo -> height = rInit->height;
    }else{
      wInfo -> height = min_height;
    }

    wInfo -> title = rInit->title;
    wInfo -> x = rInit->x;
    wInfo -> y = rInit->y;

    rmon -> count = 0;

    if(Cluster > ClusterH*ClusterW){
      rmon -> Cluster = ClusterH*ClusterW;
    }else{
      rmon -> Cluster = Cluster;
    }

    rmon -> width = wInfo ->width;
    rmon -> height = wInfo -> height;
    cal_sample_size(rmon,&sw,&sh);
    rmon->sw = sw;
    rmon->sh = sh;

    if(rInit -> scroll < 1 ){
	rmon -> scroll = 1;
    }else{
	rmon -> scroll = rInit -> scroll;
    }
}
/********** cal_window_min_size **********/
static void cal_window_min_size(rmon,min_width,min_height)
PRmon *rmon;
unsigned int *min_width;
unsigned int *min_height;
{
  unsigned int min_height1;
  unsigned int min_height2;
  unsigned int ClusterW;
  unsigned int ClusterH;
  unsigned int cycle;
  unsigned int pe;
  unsigned int GraphW;
  unsigned int GraphH;

  ClusterW = rmon -> ClusterW;
  ClusterH = rmon -> ClusterH;
  cycle = rmon -> cycle;
  pe = rmon -> pe;
  
  GraphW = cycle * SampleMinW * ClusterW + IntervalW * (ClusterW-1);
  GraphH = pe * SampleMinH * ClusterH + IntervalH * ClusterH;

  *min_width = MarginLeft + GraphW + MarginRight;
  *min_width = *min_width + *min_width / (SampleRange-1);
  *min_width = *min_width + *min_width / (SampleIntervalRange-1);

  min_height1 = MarginTop + GraphH + MarginDown;
  min_height2 = MarginTop + colors*SampleMinH + MarginDown;
  if(min_height1 <= min_height2){
    *min_height = min_height2;
  }else{
    *min_height = min_height1;
  }
}

/********** cal_sample_size **********/
static void cal_sample_size(rmon,sw,sh)
PRmon *rmon;
unsigned int *sw;
unsigned int *sh;
{
  unsigned int minW;
  unsigned int minH;
  unsigned int charH = r14Height;
  unsigned int GraphW;
  unsigned int GraphH;
  unsigned int sampleInterval;

  cal_window_min_size(rmon,&minW,&minH);
  if(rmon->width > 640 && rmon->height > 512 &&
     rmon->height >=  minH + charH * rmon->ClusterH){
    *sh = (rmon->height - (MarginTop + (IntervalH + charH) * rmon->ClusterH
			   + MarginDown)) / (rmon->pe * rmon->ClusterH);
  }else{
    *sh = (rmon->height - (MarginTop + IntervalH * rmon->ClusterH
			     + MarginDown)) / (rmon->pe * rmon->ClusterH);
  }

    sampleInterval = rmon->width/SampleIntervalRange;
  *sw = (rmon->width - rmon->width / SampleRange - 
	 (MarginLeft + IntervalW *(rmon->ClusterW-1) + 
	  sampleInterval + MarginRight)) / (rmon->cycle * rmon->ClusterW);
  if(*sw <= 0){
  *sw = (rmon->width - rmon->width / SampleRange - 
	 (MarginLeft + IntervalW *(rmon->ClusterW-1) + 
	  SampleMinInterval + MarginRight)) / (rmon->cycle * rmon->ClusterW);
  }
}

/********** get_font_flag **********/
static int get_font_flag(rmon)
PRmon *rmon;
{
  unsigned int minW;
  unsigned int minH;
  unsigned int charH=r14Height;

  cal_window_min_size(rmon,&minW,&minH);
  if(rmon->width > 640 && rmon->height > 512 &&
     rmon->height >=  minH + charH * rmon->ClusterH){
    return(1);
  }else{
    return(0);
  }
}
/********** init_draw **********/
static void init_draw(rmon)
PRmon *rmon;
{
    activateSWin(rmon->window);
    draw_frame(rmon);
    draw_cluster(rmon);
    draw_percent(rmon);
    draw_sample(rmon);
}

/**********  draw_frame **********/
static void draw_frame(rmon)
PRmon *rmon;
{

  char checker[32];
  char check[32];

  Pixmap pixmap;
  Pixmap draw_pattern;
  SWin *win;
  unsigned int width;
  unsigned int height;
  int InLeftTopX;
  int InLeftTopY;
  int InLeftDownX;
  int InLeftDownY;
  int InRightTopX;
  int InRightTopY;
  int InRightDownX;
  int InRightDownY;
  int i;
  int j;
  unsigned int GraphWidth;
  unsigned int GraphHeight;
  int F;
  unsigned int charW;
  unsigned int charH;
  unsigned int Shadow;
  unsigned int counter;
  int x;
  int y;
  int w;
  int h;

  checker[0]=0x44;
  checker[1]=0x44;
  checker[2]=0x11;
  checker[3]=0x11;

  check[0]=0x77;
  check[1]=0x77;
  check[2]=0xbb;
  check[3]=0xbb;
  check[4]=0xee;
  check[5]=0xee;
  check[6]=0xdd;
  check[7]=0xdd;

  win = rmon->window;

  width = rmon->width;
  height = rmon->height;

  InLeftTopX = FrameWidth;
  InLeftTopY = FrameWidth;
  InLeftDownX = FrameWidth;
  InLeftDownY = height - FrameWidth;
  InRightTopX = width - FrameWidth;
  InRightTopY = FrameWidth;
  InRightDownX = width - FrameWidth;
  InRightDownY = height - FrameWidth;

  set_color(rmon->window,"black");
  draw_pattern_rectangle(win,0,0,width,height,1,checker,16,2,pSet);

  if(rmon->type == black){
    set_color(win,BVerBackColor);
    draw_filled_rectangle(win,FrameWidth, FrameWidth, width-FrameWidth*2,
			  height-FrameWidth*2,1,NULL,pSet);
  }else{
    set_color(win,WVerBackColor);
    draw_filled_rectangle(win,FrameWidth, FrameWidth, width-FrameWidth*2,
			  height-FrameWidth*2,1,NULL,pSet);
  }

  set_color(win,FramePositivColor);
  draw_line(win,1,0,1,height,2,pSolid,pSet);
  draw_line(win,1,1,width,1,2,pSolid,pSet);
  draw_line(win,InRightTopX,InRightTopY,InRightDownX,InRightDownY,2,pSolid,pSet);
  draw_line(win,InLeftDownX,InLeftDownY,InRightDownX,InRightDownY,2,pSolid,pSet);

  set_color(win,FrameShadowColor);
  draw_line(win,width-1,1,width-1,height,2,pSolid,pSet);
  draw_line(win,width,height-1,1,height-1,2,pSolid,pSet);
  draw_line(win,InLeftTopX,InLeftTopY-1,InLeftDownX,InLeftDownY,2,pSolid,pSet);
  draw_line(win,InLeftTopX-1,InLeftTopY,InRightTopX,InRightTopY,2,pSolid,pSet);

  /* create claster Pixmap */

  if(rmon->width>rmon->height){
    Shadow=rmon->height/100;
  }else{
    Shadow=rmon->width/100;
  }
  if(Shadow > IntervalW /2)Shadow = IntervalW/2;

  w = rmon->sw*rmon->cycle +2;
  h = rmon->sh*rmon->pe +2;
  if(rmon->type == black){
    if(rmon->uneven == convex){
      pixmap = XCreatePixmap(win->display, win->window, w+Shadow, h+Shadow,
			     DefaultDepth(win->display,win->screen_number));
    }else{
      pixmap = XCreatePixmap(win->display, win->window, w+3, h+2,
			     DefaultDepth(win->display,win->screen_number));
    }
  }else{
    pixmap = XCreatePixmap(win->display, win->window, w, h,
			   DefaultDepth(win->display,win->screen_number));
  }    
  if(rmon->type == black){
    if(rmon->uneven == convex){
      set_color(win,BVerBackColor);
      XFillRectangle(win->display,pixmap,win->gc,0,0,w+Shadow,h+Shadow);
      set_color(win,BVerShadowColor);
      XFillRectangle(win->display,pixmap,win->gc,Shadow,Shadow,w,h);
      XSetFillStyle(win->display,win->gc,FillSolid);
      set_color(win,"black");
      XFillRectangle(win->display,pixmap,win->gc,0,0,w,h);
      set_color(win,BVerBackColor);
      XFillRectangle(win->display,pixmap,win->gc,1,1,w-2,h-2);
    }else{
      set_color(win,BVerBackColor);
      XFillRectangle(win->display,pixmap,win->gc,0,0,w+3,h+2);
      set_color(win,BVerShadowColor);
      XSetLineAttributes(win->display,win->gc,1,
			 LineSolid,CapButt,JoinMiter);


      /* left vertical */
      XDrawLine(win->display,pixmap,win->gc,0,1,0,h+1);
      XDrawLine(win->display,pixmap,win->gc,1,1,1,h+1);

      /* top horizontal */
      XDrawLine(win->display,pixmap,win->gc,1,0,w+3,0);
      XDrawLine(win->display,pixmap,win->gc,1,1,w+2,1);
      set_color(win,BVerBackColor);

      /* right vertival */
      XDrawLine(win->display,pixmap,win->gc,w+1,2,w+1,h-2);
      set_color(win,BVerPositivColor);
      XDrawLine(win->display,pixmap,win->gc,w+1,2,w+1,h);
      XDrawLine(win->display,pixmap,win->gc,w+2,1,w+2,h);

      /* down horizontal */
      XDrawLine(win->display,pixmap,win->gc,1,h,w+2,h);
      XDrawLine(win->display,pixmap,win->gc,0,h+1,w+2,h+1);
    }      
  }else{
    set_color(win,WVerBackColor);
    XFillRectangle(win->display,pixmap,win->gc,0,0,w,h);
    set_color(win,"black");
    XFillRectangle(win->display,pixmap,win->gc,0,0,w,h);
    set_color(win,WVerBackColor);
    XFillRectangle(win->display,pixmap,win->gc,1,1,w-2,h-2);

  }
  

  GraphWidth = rmon->cycle*rmon->sw;
  GraphHeight = rmon->pe*rmon->sh;
  F = get_font_flag(rmon);
  get_font_size(rmon->window, &charW, &charH);

  counter = 0;

  for(j=rmon->ClusterH-1;j>=0;j--){
    for(i=0;i<rmon->ClusterW;i++){
      if(counter < rmon->Cluster){
	x = MarginLeft+(GraphWidth*i+IntervalW*i)-1;
	y = MarginTop+(GraphHeight+IntervalH+charH*F)*j-1;
	if(rmon->type == black){
	  if(rmon->uneven == convex){
	    XCopyArea(win->display,pixmap,win->window,win->gc,0,0,
		      w+Shadow,h+Shadow,x,y);
	  }else{
	    XCopyArea(win->display,pixmap,win->window,win->gc,0,0,
		      w+3,h+2,x-1,y-1);
	  }
	}else{
	  XCopyArea(win->display,pixmap,win->window,win->gc,0,0,
		    w,h,x,y);
	}
	counter++;
      }
    }
  }
  XFreePixmap(win->display,pixmap);

  /* create clear pixmap */
  w = rmon->sw*(rmon->cycle/rmon->scroll);
  h = rmon->sh*rmon->pe;
  pixmap = XCreatePixmap(win->display, win->window, w, h,
			   DefaultDepth(win->display,win->screen_number));

  if(rmon->type == black){
    set_color(win,BVerBackColor);
  }else{
    set_color(win,WVerBackColor);
  }

  XFillRectangle(win->display,pixmap,win->gc,0,0,w,h);
  rmon->pixmap[colors*2] = pixmap;

}
/********** draw_cluster **********/
static void draw_cluster(rmon)
PRmon *rmon;
{

  unsigned int charW;
  unsigned int charH;
  unsigned int GraphWidth;
  unsigned int GraphHeight;
  unsigned int Shadow;
  int F;
  int i;
  int j;
  int x;
  int y;
  int stringW;
  unsigned int clusterN=0;
  char string[10];
  char *num;


  if(get_font_flag(rmon)!=0){
    set_color(rmon->window,BVerStringColor);
    get_font_size(rmon->window,&charW,&charH);
    GraphWidth = rmon->cycle * rmon->sw;
    GraphHeight = rmon->pe * rmon->sh;

    if(rmon->width>rmon->height){
      Shadow=rmon->height/100;
    }else{
      Shadow=rmon->width/100;
    }
  if(Shadow > IntervalW /2)Shadow = IntervalW/2;
    for(i=rmon->ClusterH;i>0;i--){
      for(j=0;j < rmon->ClusterW;j++){
	if(clusterN < rmon->Cluster){
	  strcpy(string,"#");
	  num = get_number_string(clusterN);
	  strcat(string,num);
	  stringW = strlen(string) * charW;
	  
	  x = MarginLeft + (GraphWidth - stringW) /2 
	    + (GraphWidth + IntervalW) * j;
	  y = MarginTop +GraphHeight * i + (IntervalH +charH) *(i-1);
	  if(rmon->type == black){
	    if(rmon->uneven == convex){
	      set_color(rmon->window,BVerShadowColor);
	      draw_string(rmon->window,x+Shadow,y+Shadow,string,"r14",pSet);
	      set_color(rmon->window,BVerStringColor);
	      draw_string(rmon->window,x,y,string,"r14",pSet);
	    }else{
	      set_color(rmon->window,BVerStringColor);
	      draw_string(rmon->window,x,y,string,"r14",pSet);
	    }
	  }else{
	    set_color(rmon->window,WVerStringColor);
	    draw_string(rmon->window,x,y,string,"r14",pSet);
	  }
	  free(num);
	  clusterN++;
	}
      }
    }
  }
}


/********** draw_sample **********/
static void draw_sample(rmon)
PRmon *rmon;
{
  unsigned int SampleMinHeight;
  unsigned int NowSampleHeight;
  unsigned int SampleHeight;
  unsigned int SHeight;
  unsigned int SWidth;
  unsigned int Shadow;
  int SampleX;
  int SampleY;
  int i;
  int sx;
  int sy;
  int SampleLeftTopX;
  int SampleLeftTopY;
  int SampleRightTopX;
  int SampleRightTopY;
  int SampleLeftDownX;
  int SampleLeftDownY;
  int SampleRightDownX;
  int SampleRightDownY;

  SHeight = cal_sample_height(rmon);
  SWidth  = rmon->width / SampleRange;
  cal_sample_position(rmon,&SampleX,&SampleY);

  set_color(rmon->window,"Black");
/*
  if(rmon->width>rmon->height){
    Shadow=rmon->height/100;
  }else{
    Shadow=rmon->width/100;
  }
*/
  if(rmon->type == black){
    SampleLeftTopX= SampleX -1;
    SampleLeftTopY = SampleY -1;
    SampleRightTopX= SampleX-1 + SWidth +1;
    SampleRightTopY= SampleY -1;
    SampleLeftDownX= SampleX -1;
    SampleLeftDownY= SampleY-1 + SHeight*colors +2;
    SampleRightDownX = SampleX-1 + SWidth +1;
    SampleRightDownY = SampleY-1 + SHeight*colors +2;

    for(i=0;i<colors;i++){
      sx = SampleX;
      sy = SampleY+SHeight*i;
      
      set_color(rmon->window,*(rmon->Color+i));
      draw_filled_rectangle(rmon->window,sx,sy,SWidth,SHeight,1,NULL,pSet);
    }
    
    set_color(rmon->window,"Black");
    draw_line(rmon->window,SampleLeftTopX,SampleLeftTopY,
	      SampleRightTopX+1,SampleRightTopY,2,pSolid,pSet);
    draw_line(rmon->window,SampleLeftTopX,SampleLeftTopY,
	      SampleLeftDownX,SampleLeftDownY,2,pSolid,pSet);
    /*sample mark */
    set_color(rmon->window,BVerPositivColor);
    draw_line(rmon->window,SampleRightTopX,SampleRightTopY,
	      SampleRightDownX,SampleRightDownY,3,pSolid,pSet);
    draw_line(rmon->window,SampleRightDownX,SampleRightDownY,
	      SampleLeftDownX,SampleLeftDownY,2,pSolid,pSet);
    set_color(rmon->window,BVerBackColor);
    draw_line(rmon->window,SampleRightTopX-1,SampleRightTopY,
	      SampleRightDownX-1,SampleRightDownY-2,1,pSolid,pSet);

  }else{
    draw_filled_rectangle(rmon->window,SampleX-1,SampleY-1,SWidth+2,SHeight*colors+2,1,NULL,pSet);
    for(i=0;i<colors;i++){
      sx = SampleX;
      sy = SampleY+SHeight*i;
      
      set_color(rmon->window,*(rmon->Color+i));
      draw_filled_rectangle(rmon->window,sx,sy,SWidth,SHeight,1,NULL,pSet);
    }
  }
}

/********** cal_sample_height **********/
static unsigned int cal_sample_height(rmon)
PRmon *rmon;
{
  unsigned int SampleMinHeight;
  unsigned int NowSampleHeight;
  unsigned int SampleHeight;
  unsigned int SHeight;
  unsigned int Mwidth;
  unsigned int Mheight;
  int F;

  F = get_font_flag(rmon);

  if(rmon->height <= BorderHeight){
    SampleHeight = MarginTop+((rmon->pe*rmon->sh)+IntervalH+14*F)*
      rmon->ClusterH +MarginDown - (SampleMarginDown+SampleMarginTop);
  }else{
    SampleMinHeight = BorderHeight - SampleMarginTop -SampleMarginDown;
    NowSampleHeight = rmon->height /3*2;
    if(SampleMinHeight >= NowSampleHeight){
      SampleHeight = SampleMinHeight;
    }else{
      SampleHeight = NowSampleHeight;
    }
  }
  SHeight = SampleHeight / colors;
  return(SHeight);
}
/********** cal_sample_position **********/
static void cal_sample_position(rmon,SampleX,SampleY)
PRmon *rmon;
int *SampleX;
int *SampleY;
{

  unsigned int SHeight;
  int F;
  unsigned int sampleInterval;

  F=get_font_flag(rmon);
  SHeight = cal_sample_height(rmon);

  *SampleY = rmon->height / 2 - SHeight*colors /2;
/*  *SampleY = MarginTop + rmon->pe * rmon->sh * rmon->ClusterH + 
    IntervalH * rmon->ClusterH + 14 * rmon->ClusterH*F + MarginDown -
      (SampleMarginDown + SHeight * colors);
*/
  if(rmon->width/SampleIntervalRange < SampleMinInterval){
    sampleInterval = SampleMinInterval;
  }else{
    sampleInterval = rmon->width/SampleIntervalRange;
  }

  *SampleX = MarginLeft + (rmon->cycle * rmon->sw) * rmon->ClusterW
    + IntervalW * (rmon->ClusterW-1)+ sampleInterval;
}
/********** change_num **********/
static char *get_number_string(n)
int n;
{
    char c1[10];
    char c2[10];
    char *c3;
    int integer;
    int i = 0;
    int len ;
    integer = n;
    while(1){
	*(c1+i) = (integer % 10) + '0';
	integer = integer / 10;
	i++;
	if(integer==0)break;
    }
    *(c1+i)= '\0';
    len=strlen(c1);
    i=0;
    while(len!=i){
	c2[i]= c1[len-i-1];
	i++;
    }
    c2[i]='\0';
    c3 = (char *) malloc(sizeof(char)*len);
    strcpy(c3,c2);
    return(c3);
}

/********** draw_percent **********/
static void draw_percent(rmon)
PRmon *rmon;
{
    int charH;
    int charW;
    int x;
    int y100p;
    int y50p;
    int y0p;
    int sx;
    int sy;

    cal_sample_position(rmon,&sx,&sy);
    get_font_size(rmon->window,&charW,&charH);
    x = sx + rmon->width / SampleRange + 5;
    y100p = sy -4;
    y50p = sy + cal_sample_height(rmon)*10 -charH/2 -2;
    y0p = sy + cal_sample_height(rmon) * colors - charH;
    if(rmon->type == black){
      set_color(rmon->window,BVerStringColor);
    }else{
      set_color(rmon->window,WVerStringColor);
    }
      draw_string(rmon->window,x,y100p,"100%","r14",pSet);
      draw_string(rmon->window,x,y50p,"50%","r14",pSet);
      draw_string(rmon->window,x,y0p,"0%","r14",pSet);

}

/********** set_palette **********/
static void set_palette(rmon)
PRmon *rmon;
{
    *(rmon->Color+0) = "red";
    *(rmon->Color+1) = "OrangeRed";
    *(rmon->Color+2) = "tomato";
    *(rmon->Color+3) = "coral";
    *(rmon->Color+4) = "DarkOrange";
    *(rmon->Color+5) = "orange";
    *(rmon->Color+6) = "gold";
    *(rmon->Color+7) = "#ffffeeee0000"; 
    *(rmon->Color+8) = "#ddddffff0000"; 
    *(rmon->Color+9) = "GreenYellow";  
    *(rmon->Color+10) = "LawnGreen";
    *(rmon->Color+11) ="green";
    *(rmon->Color+12) = "#1111ffff9999"; 
    *(rmon->Color+13) = "#0000ddddffff"; 
    *(rmon->Color+14) = "#0000bbbbffff"; 
    *(rmon->Color+15) = "#00009999ffff"; 
    *(rmon->Color+16) = "#00007777ffff"; 
    *(rmon->Color+17) = "#00005555ffff"; 
    *(rmon->Color+18) = "#00000000ffff"; 
    *(rmon->Color+19) = "MediumBlue"; 
}
