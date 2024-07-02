#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/***************************************************/
/*  1992.4.13                                      */
/*  runtime-monitor                                */
/*  version 12                                     */
/***************************************************/

#include "rmon20.h"

#define   ULONG_MAX        4294967295 /* ulong MAX for psi3 */
#define   FrameWidth       10
#define   MarginTop        (FrameWidth+10)
#define   MarginLeft       (FrameWidth+38)
#define   MarginRight      (FrameWidth+40)
#define   MarginDown       (FrameWidth+15)
#define   SampleMarginDown (FrameWidth+3)
#define   SampleMarginTop  (FrameWidth+3)
#define   Interval         50
#define   SampleInterval   15
#define   BorderHeight     300

#define   SampleMinInterval 5
#define   SampleIntervalRange       20
#define   SampleRange      20
#define   SampleInterval   15
#define   SampleMinW       1
#define   SampleMinH       1

#define   BVerBackColor         (char *)"DarkSlateGray"
#define   BVerShadowColor       (char *)"black"
#define   BVerStringColor       (char *)"white"
#define   BVerStringShadowColor (char *)"black"

#define   WVerBackColor         (char *)"white"
#define   WVerShadowColor       (char *)"white"
#define   WVerStringColor       (char *)"black"
#define   WVerStringShadowColor (char *)"white"
#define   BVerPositivColor      (char *)"LightSlateGray"
#define   FrameShadowColor       (char *)"black"
#define   FramePositivColor       (char *)"lavender"

#define   colors 20

static void killPixM();
static void draw_frame();
static void reshape_window();
static void copy_pixmap();
static void set_palette();
static void draw_percent();
static void init_draw();
static void get_init_data();
static void draw_pe();
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

/********** resetRinit **********/
void resetRinit(rinit)
RInit *rinit;
{
    rinit -> x = 0;
    rinit -> y = 0;
    rinit -> SWidth = 40;
    rinit -> SHeight = 25;
    rinit -> width = 690;
    rinit -> height = 250;
    rinit -> scroll = 4;
    rinit -> cycle = 10;
    rinit -> displayType = NULL;
    rinit -> type = white;
    rinit -> uneven = convex;
    rinit -> title = "Runtime Monitor";
}

/********** create_rmonitor **********/
Rmon *create_rmonitor(pe,rInit)
unsigned int pe;
RInit rInit;
{
    InitInfo wInfo;
    Rmon     *rmon;
    SWin    *win;

    resetWinit(&wInfo);
    rmon = (Rmon *)malloc(sizeof(Rmon));
    get_init_data(pe,&rInit,&wInfo,rmon);
    set_palette(rmon);
    win = createSWin(wInfo);
    rmon->window = win;
    createPixmap(rmon);
    init_draw(rmon);
    return(rmon);
}

/********** activate_rmonitor **********/
void activate_rmonitor(rmon)
Rmon *rmon;
{
  activateSWin(rmon->window);
}

/********** deactivate_rmonitor **********/
void deactivate_rmonitor(rmon)
Rmon *rmon;
{
  deactivateSWin(rmon->window);
}

/********** kill_rmonitor **********/
void kill_rmonitor(rmon)
Rmon *rmon;
{
  killPixM(rmon);
  killSWin(rmon->window);
  free(rmon);
}
/********** draw_rmonitor **********/
void draw_rmonitor(rmon,data)
Rmon *rmon;
unsigned short *data;
{
    int count;
    flush(rmon->window);
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
Rmon *rmon;
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
    pe = rmon->pe;
    if (pe<colors){
	pe0=colors;
    }else{
	pe0=pe;
    }

    killPixM(rmon);
    rmon->count = 0;
    rmon -> width = width;
    rmon -> height = height;
    cal_sample_size(rmon,&sw,&sh);
    rmon->sw = sw;
    rmon->sh = sh;
/*
    GraphWidth = rmon->cycle * rmon->sw;
    GraphHeight = pe0 * rmon->sh;
    rmon -> sx = MarginLeft + GraphWidth + Interval;
    rmon -> sy = MarginTop + GraphHeight + MarginDown -	(SampleMarginDown + rmon ->sh * colors);
*/

    createPixmap(rmon);
    clear_area(rmon->window,0,0,width,height);
    draw_frame(rmon);
    draw_pe(rmon);
    draw_percent(rmon);
    draw_sample(rmon);
  }
}
/********** killPixM **********/
static void killPixM(rmon)
Rmon *rmon;
{
  int i;

  for(i=0;i<colors;i++)
    {
      XFreePixmap(rmon->window->display,rmon->pixmap[i]);
      /****** !!! ********/
    }
}

/********** createPixmap **********/
static void createPixmap(rmon)
Rmon *rmon;
{
  Pixmap pixmap1;
  Pixmap pixmap2;
  int i;
  SWin *win;
  unsigned int sh;
  unsigned int sw;
  double twidth;
  double theight;
  double gcWidth;
  double gcHeight;
  unsigned int gcYTop;
  unsigned int gcYDown;
  win = rmon->window;

  for(i=0;i<colors;i++){
    pixmap1 = XCreatePixmap(win->display, win->window, rmon->sw, rmon->sh,
			   DefaultDepth(win->display,win->screen_number));
    pixmap2 = XCreatePixmap(win->display, win->window, rmon->sw, rmon->sh,
			   DefaultDepth(win->display,win->screen_number));
    
    set_color(rmon->window,*(rmon->color+(colors-1-i)));
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
Rmon *rmon;
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
Rmon *rmon;
{
    int x1;
    int x2;
    int x3;
    int y1;
    int y2;
    int y3;
    unsigned int w;
    unsigned int h;
    unsigned int w1;
    unsigned int h1;
    unsigned int scroll;

    scroll = rmon->cycle / rmon->scroll;
    if(scroll < 1)scroll= 1;
      
    if(rmon->count == rmon->cycle){
	x1 = MarginLeft + scroll * rmon->sw;
	y1 = MarginTop;
	w = (rmon->cycle - scroll)*rmon->sw;
	h = rmon -> pe * rmon ->sh;
	x2 = MarginLeft;
	y2 = MarginTop;
	x3 = MarginLeft + w;
	y3 = MarginTop;
	w1 = scroll * rmon -> sw;
	h1 = rmon -> pe * rmon-> sh;

	transfer(rmon->window,x1,y1,w,h,x2,y2);
	if(rmon->type == black){
	  set_color(rmon->window,BVerBackColor);
	}else{
	  set_color(rmon->window,WVerBackColor);
	}
	draw_filled_rectangle(rmon->window,x3,y3,w1,h1,1,NULL,pSet);

	rmon -> count = rmon -> cycle - scroll;
	
    }
}
/********** draw_data **********/
static void draw_data(rmon,data)
Rmon *rmon;
unsigned short *data;
{
    int counter = 0;
    int pos = 0;
    int i;
    int p;
    unsigned short a1;
    unsigned short a2;
    unsigned short na1;
    unsigned short na2;
    unsigned short gc;
    unsigned long a;
    unsigned long na;
    unsigned long total;

    for(i=0;i!=rmon->pe;i++){
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
	  if(total/colors == 0){
	    p = na*10 / total;
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
Rmon *rmon;
int p;
int counter;
unsigned short gc;
{
    int x1;
    int y1;
    int p0;

    x1 = MarginLeft + rmon -> count * rmon -> sw;
    y1 = MarginTop + rmon -> sh * counter;
    if(gc==1){
      p0 = p+colors;
    }else{
      p0 =  p;
    }

    copy_pixmap(rmon,p0,x1,y1);
}

/********** copy_pixmap ********/
static void copy_pixmap(rmon,p,x,y)
Rmon *rmon;
int p;
int x;
int y;
{
  SWin *win;
  win = rmon->window;
  XCopyArea(win->display,rmon->pixmap[p],win->window,win->gc,0,0,rmon->sw,rmon->sh,x,y);
  
}

/********** cal_window_min_size **********/
static void cal_window_min_size(rmon,min_width,min_height)
Rmon *rmon;
unsigned int *min_width;
unsigned int *min_height;
{
  unsigned int min_height1;
  unsigned int min_height2;
  unsigned int cycle;
  unsigned int pe;
  unsigned int GraphW;
  unsigned int GraphH;
  unsigned int graphWidth;
  unsigned int sampleWidth;
  unsigned int sampelInterval;

  cycle = rmon -> cycle;
  pe = rmon -> pe;
  
  GraphW = cycle * SampleMinW;
  GraphH = pe * SampleMinH;

  graphWidth = MarginLeft + GraphW + MarginRight;

  *min_width = graphWidth *SampleRange *SampleIntervalRange /
    (SampleRange *SampleIntervalRange - (SampleRange +SampleIntervalRange));
/*
  printf("width %d Y %d a %d b %d int %d sw %d cycle %d \n", 
	 *min_width ,graphWidth ,SampleRange ,SampleIntervalRange,
	 *min_width/SampleIntervalRange,*min_width/SampleRange,GraphW);
*/
  min_height1 = MarginTop + GraphH + MarginDown;
  min_height2 = MarginTop + colors*SampleMinH + MarginDown;
  if(min_height1 <= min_height2){
    *min_height = min_height2;
  }else{
    *min_height = min_height1;
  }
}

/********** get_init_data **********/
static void get_init_data(pe,rInit,wInfo,rmon)
unsigned int pe;
RInit *rInit;
InitInfo *wInfo;
Rmon *rmon;
{
    int min_width;
    int min_height;
    unsigned int sw;
    unsigned int sh;


/*
    if (pe<colors){
      min_height = MarginTop + colors + MarginDown;
    }else{
      min_height = MarginTop + MarginDown + pe;
    }
    min_width = MarginLeft + MarginRight + Interval +
      (rInit-> cycle+1) * 1;
*/    
    rmon -> count = 0;
    rmon -> pe = pe;
    rmon -> cycle = rInit -> cycle;
    rmon -> type = rInit -> type;
    rmon -> uneven = rInit -> uneven;
    rmon -> displayType = rInit -> displayType;

    cal_window_min_size(rmon,&min_width,&min_height);
    wInfo->min_width = min_width;
    wInfo->min_height = min_height;

    if(rInit->width > min_width){
      wInfo->width = rInit->width;
    }else{
      wInfo->width = min_width;
    }
    if(rInit -> height > min_height){
      wInfo->height = rInit->height;
    }else{
      wInfo->height = min_height;
    }

    rmon -> width = wInfo ->width;
    rmon -> height = wInfo -> height;

    if(rInit -> scroll < 1 ){
	rmon -> scroll = 1;
    }else{
	rmon -> scroll = rInit -> scroll;
    }
    cal_sample_size(rmon,&sw,&sh);
    rmon -> sw = sw;
    rmon -> sh = sh;

    wInfo -> title = rInit->title;
    wInfo -> x = rInit->x;
    wInfo -> y = rInit->y;


}

/********** cal_sample_size **********/
static void cal_sample_size(rmon,sw,sh)
Rmon *rmon;
unsigned int *sw;
unsigned int *sh;
{
  unsigned int sampleInterval;
  unsigned int sampleWidth;
/*
  *sw = (rmon->width - (MarginLeft + Interval + MarginRight))
        / (rmon->cycle);
*/
  *sh = (rmon->height-(MarginTop + MarginDown)) / rmon->pe;

  sampleInterval = rmon->width/SampleIntervalRange;
  sampleWidth = rmon->width / SampleRange;

  *sw = (rmon->width - 
	 (MarginLeft + sampleInterval +sampleWidth + MarginRight)) / 
	   (rmon->cycle);

  if(*sw <= 0){
    *sw = 1;
/*
    *sw = (rmon->width - rmon->width / SampleRange - 
	   (MarginLeft + SampleMinInterval + MarginRight)) / (rmon->cycle);
*/
  }
}  

/********** init_draw **********/
static void init_draw(rmon)
Rmon *rmon;
{
    activateSWin(rmon->window);
    draw_frame(rmon);
    draw_pe(rmon);
    draw_percent(rmon);
    draw_sample(rmon);
}

/**********  draw_frame **********/
static void draw_frame(rmon)
Rmon *rmon;
{
  char checker[32];


  Pixmap pixmap;
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
  unsigned int Shadow;
  unsigned int w;
  unsigned int h;

  checker[0]=0x44;
  checker[1]=0x44;
  checker[2]=0x11;
  checker[3]=0x11;

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

  if(rmon->width>rmon->height){
    Shadow=rmon->height/100;
  }else{
    Shadow=rmon->width/100;
  }

  set_color(rmon->window,"black");
  draw_pattern_rectangle(win,0,0,width,height,1,checker,16,2,pSet);

  if(rmon->type == black){
    set_color(rmon->window,BVerBackColor);
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

  draw_filled_rectangle(rmon->window, MarginLeft-1, MarginTop-1,
			rmon->sw*rmon->cycle+2, rmon->sh*rmon->pe+2, 
			1, NULL, pSet);
  if(rmon->type == black){
    if(rmon->uneven == convex){
      set_color(win,BVerShadowColor);
      draw_filled_rectangle(rmon->window, MarginLeft-1+Shadow, 
			    MarginTop-1+Shadow,rmon->sw*rmon->cycle+2,
			    rmon->sh*rmon->pe+2, 1, NULL, pSet);
      set_color(win,BVerBackColor);
    }else{
      w = rmon->sw*rmon->cycle + 2;
      h = rmon->sh * rmon->pe + 2;
      set_color(win,BVerShadowColor);

      /* top horizontal */
      draw_line(win,MarginLeft-1,MarginTop-2,MarginLeft -2 + w+3,
		MarginTop-2,1,pSolid,pSet);
      draw_line(win,MarginLeft-2,MarginTop-1,MarginLeft -2 + w+3,
		MarginTop-1,1,pSolid,pSet);

      /* left vertical */
      draw_line(win,MarginLeft-2,MarginTop-1,MarginLeft-2,MarginTop -2 + h+1,
		1,pSolid,pSet);
      draw_line(win,MarginLeft-1,MarginTop-2,MarginLeft-1,MarginTop -2 + h+1,
		1,pSolid,pSet);

      set_color(win,BVerPositivColor);

      /* down horizontal */
      draw_line(win,MarginLeft-1,MarginTop-2 + h,MarginLeft -2 + w+2,
		MarginTop-2 +h ,1,pSolid,pSet);
      draw_line(win,MarginLeft-2,MarginTop-1 +h,MarginLeft -2 + w+2,
		MarginTop-1 +h ,1,pSolid,pSet);

      /* right vertival */
      draw_line(win,MarginLeft-2 +w+1 ,MarginTop,MarginLeft-2 +w+1,
		MarginTop -2 + h, 1,pSolid,pSet);
      draw_line(win,MarginLeft-1 +w+1 ,MarginTop-1,MarginLeft-1 +w+1,
		MarginTop -2 + h, 1,pSolid,pSet);
      set_color(win,BVerBackColor);      
      draw_line(win,MarginLeft-3 +w+1 ,MarginTop,MarginLeft-3 +w+1,
		MarginTop -3 + h, 1,pSolid,pSet);
    }
  }else{
    set_color(win,WVerBackColor);
  }
    draw_filled_rectangle(rmon->window, MarginLeft, MarginTop,
			  rmon->sw*rmon->cycle, rmon->sh*rmon->pe, 
			  1, NULL, pSet);
}
/********** draw_pe **********/
static void draw_pe(rmon)
Rmon *rmon;
{
    int drawHeight;
    int drawCount;
    int x;
    int y;
    int i;
    char *num;
    int len;
    int charW;
    int charH;
    int count=0;
    get_font_size(rmon->window,&charW,&charH);
    drawHeight = charH / rmon->sh + 1;
    y = MarginTop+ rmon ->sh/2 - charH/2-2;
    x = MarginLeft -7;
    drawCount = drawHeight;
    if(rmon->type == black){
      set_color(rmon->window,BVerStringColor);
    }else{
      set_color(rmon->window,WVerStringColor);
    }
    
    for(i = 0;i < rmon->pe; i++){
	if (drawCount == drawHeight && drawHeight + count <= rmon->pe-1){
	    num = get_number_string(count);
	    len = strlen(num);
	    draw_string(rmon->window,x-(charW*len),y+count*rmon->sh,num,"r14",pSet);
	    free(num);
	    drawCount=0;
          }
        drawCount++;
        count++;	
    }
    num = get_number_string(rmon->pe-1);
    len = strlen(num);        
    draw_string(rmon->window,x-(charW*len),y+(rmon->pe-1)*rmon->sh,num,"r14",pSet);
    free(num);
}


/********** draw_sample **********/
static void draw_sample(rmon)
Rmon *rmon;
{
  unsigned int SampleMinHeight;
  unsigned int NowSampleHeight;
  unsigned int SampleHeight;
  unsigned int SHeight;
  unsigned int SWidth;
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
/*
  SWidth  = rmon->sw;
*/
  cal_sample_position(rmon,&SampleX,&SampleY);


  SampleLeftTopX= SampleX ;
  SampleLeftTopY = SampleY;
  SampleRightTopX= SampleX + SWidth ;
  SampleRightTopY= SampleY ;
  SampleLeftDownX= SampleX ;
  SampleLeftDownY= SampleY + SHeight*colors ;
  SampleRightDownX = SampleX + SWidth ;
  SampleRightDownY = SampleY + SHeight*colors ;

  if(rmon->type == black){

    for(i=0;i<colors;i++){
      sx = SampleX;
      sy = SampleY+SHeight*i;
      
      set_color(rmon->window,*(rmon->color+i));
      draw_filled_rectangle(rmon->window,sx,sy,SWidth,SHeight,1,NULL,pSet);
    }

    /*sample brank mark */

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

    set_color(rmon->window,"DarkSlateGray");
    draw_line(rmon->window,SampleRightTopX-1,SampleRightTopY,
	      SampleRightDownX-1,SampleRightDownY-2,1,pSolid,pSet);

  }else{
    set_color(rmon->window,"Black");
    draw_filled_rectangle(rmon->window,SampleX-1,SampleY-1,
			  SWidth+2,SHeight*colors+2,1,NULL,pSet);
    for(i=0;i<colors;i++){
      sx = SampleX;
      sy = SampleY+SHeight*i;
      
      set_color(rmon->window,*(rmon->color+i));
      draw_filled_rectangle(rmon->window,sx,sy,SWidth,SHeight,1,NULL,pSet);
    }
  }
/**********
    int i;
    int x;
    int y;

    
    for (i=0;i<colors;i++){
	x = rmon->sx;
	y = rmon->sy + rmon -> sh * i;

	copy_pixmap(rmon,colors-1-i,x,y);
    }
**********/
}
/********** cal_sample_height **********/
static unsigned int cal_sample_height(rmon)
Rmon *rmon;
{
  unsigned int PeH;
  unsigned int SampleMinHeight;
  unsigned int NowSampleHeight;
  unsigned int SampleHeight;
  unsigned int SHeight;

  if(rmon->height <= BorderHeight){
    SampleHeight = (MarginTop+rmon->pe*rmon->sh+MarginDown - (SampleMarginTop+SampleMarginDown));
  }else{
    SampleMinHeight = BorderHeight -(SampleMarginTop+SampleMarginDown);
    NowSampleHeight = rmon->height /3 *2;
    if(SampleMinHeight>=NowSampleHeight){
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
Rmon *rmon;
int *SampleX;
int *SampleY;
{
  unsigned int SHeight;
  unsigned int sampleInterval;
  int Pe;
  
  if(rmon->pe < colors){
    Pe = colors;
  }else{
    Pe = rmon->pe;
  }

  SHeight = cal_sample_height(rmon);
  *SampleY = rmon->height / 2 - SHeight*colors /2;
/*
  *SampleY = MarginTop + rmon->pe * rmon->sh + MarginDown 
    -(SampleMarginDown +SHeight * colors);
*/

  if(rmon->width/SampleIntervalRange < SampleMinInterval){
    sampleInterval = SampleMinInterval;
  }else{
    sampleInterval = rmon->width/SampleIntervalRange;
  }
  
  *SampleX = MarginLeft + rmon->cycle * rmon->sw
    + sampleInterval;
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
Rmon *rmon;
{
    int charH;
    int charW;
    int x;
    int yTop;
    int yDown;
    int yCenter;
    int sx;
    int sy;
    unsigned int sh;
    cal_sample_position(rmon,&sx,&sy);
    sh = cal_sample_height(rmon);
    if(rmon->type == black){
      set_color(rmon->window,BVerStringColor);
    }else{
      set_color(rmon->window,WVerStringColor);
    }
    get_font_size(rmon->window,&charW,&charH);
    x = sx + rmon->width / SampleRange + 5;
    yTop = sy -4;
    yCenter = sy + sh * colors/2 - charH/2-2;

    yDown = sy + sh * colors - charH;
    draw_string(rmon->window,x,yTop,"100%","r14",pSet);
    draw_string(rmon->window,x,yCenter,"50%","r14",pSet);
    draw_string(rmon->window,x,yDown,"0%","r14",pSet);

}

/********** set_palette **********/
static void set_palette(rmon)
Rmon *rmon;
{
/*
    *(rmon->color) = "red";
    *(rmon->color+1) = "orange red";
    *(rmon->color+2) = "orange";
    *(rmon->color+3) = "Gold";
    *(rmon->color+4) = "yellow";
    *(rmon->color+5) = "green yellow";
    *(rmon->color+6) = "green";
    *(rmon->color+7) = "SpringGreen3";
    *(rmon->color+8) = "DeepSkyBlue3";
    *(rmon->color+9) = "blue";

    *(rmon->color  ) = "#ffff000037dc";
    *(rmon->color+1) = "#f3c066bc0000";
    *(rmon->color+2) = "#e100abe00000";
    *(rmon->color+3) = "#ea60f6180000";
    *(rmon->color+4) = "#9c40f6180000";
    *(rmon->color+5) = "#0000f2307530";
    *(rmon->color+6) = "#0000c3509c40";
    *(rmon->color+7) = "#00009c40c350";
    *(rmon->color+8) = "#00004e20c350";
    *(rmon->color+9) = "#00000000ffff";
*/

    *(rmon->color+0) = "red";
    *(rmon->color+1) = "OrangeRed";
    *(rmon->color+2) = "tomato";
    *(rmon->color+3) = "coral";
    *(rmon->color+4) = "DarkOrange";
    *(rmon->color+5) = "orange";
    *(rmon->color+6) = "gold";
    *(rmon->color+7) = "#ffffeeee0000"; 
    *(rmon->color+8) = "#ddddffff0000"; 
    *(rmon->color+9) = "GreenYellow";  
    *(rmon->color+10) = "LawnGreen";
    *(rmon->color+11) ="green";
    *(rmon->color+12) = "#1111ffff9999"; 
    *(rmon->color+13) = "#0000ddddffff"; 
    *(rmon->color+14) = "#0000bbbbffff"; 
    *(rmon->color+15) = "#00009999ffff"; 
    *(rmon->color+16) = "#00007777ffff"; 
    *(rmon->color+17) = "#00005555ffff"; 
    *(rmon->color+18) = "#00000000ffff"; 
    *(rmon->color+19) = "MediumBlue"; 

}


