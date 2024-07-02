#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/***************************************************/
/*  1992.1.24                                      */
/*  average-runtime-monitor                        */
/*  version 1                                      */
/***************************************************/

#include "amon.h"

#define PeN 8

#define   MarginTop 30
#define   MarginLeft 48
#define   MarginRight 70
#define   MarginDown 25
#define   SampleMarginDown 20
#define   Interval 20
#define   FrameWidth 10

static void draw_frame();
static void reshape_window();
static void set_palette();
static void draw_percent();
static void init_draw();
static void get_init_data();
static void scroll_graph();
static void draw_sample();
static void draw_data();
static void draw_one_sample();

/********** resetAinit **********/
void resetAinit(rinit)
AInit *rinit;
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
    rinit -> title = "Average Runtime Monitor";
}

/********** create **********/
Amon *create_amonitor(rInit)
AInit rInit;
{
    InitInfo wInfo;
    Amon     *rmon;
    SWin    *win;

    resetWinit(&wInfo);
    rmon = (Amon *)malloc(sizeof(Amon));
    get_init_data(&rInit,&wInfo,rmon);
    set_palette(rmon);
    win = createSWin(wInfo);
    rmon->window = win;

    init_draw(rmon);
    return(rmon);
}    

/********** activate **********/
void activate_amonitor(rmon)
Amon *rmon;
{
  activateSWin(rmon->window);
}

/********** deactivate **********/
void deactivate_amonitor(rmon)
Amon *rmon;
{
  deactivateSWin(rmon->window);
}

/********** kill **********/
void kill_amonitor(rmon)
Amon *rmon;
{
  killSWin(rmon->window);
  free(rmon);
}

/********** draw **********/
void draw_amonitor(rmon,data)
Amon *rmon;
unsigned short *data;
{
    int count;
    reshape_window(rmon);
    scroll_graph(rmon);
    count = rmon->count;
    draw_data(rmon,data);   /*  !!!!  */
    rmon->count = count + 1;

}

/********** reshape_window **********/
static void reshape_window(rmon)
Amon *rmon;
{
  int reshape=NO;
  unsigned int width;
  unsigned int height;


  unsigned int GraphWidth;
  unsigned int GraphHeight;

  reshape = get_reshape(rmon->window,&width,&height);
  
  if (reshape == YES && (width != rmon->width || height != rmon->height)){

    rmon->count = 0;
    rmon -> width = width;
    rmon -> height = height;
    rmon->sw = (rmon->width -(MarginLeft +Interval+MarginRight)) / (rmon->cycle +1) ;
    rmon->sh = (rmon->height-(MarginTop + MarginDown)) / PeN;
    GraphWidth = rmon->cycle * rmon->sw;
    GraphHeight = PeN * rmon->sh;
    rmon -> sx = MarginLeft + GraphWidth + Interval;
    rmon -> sy = MarginTop + GraphHeight + MarginDown -	(SampleMarginDown + rmon ->sh * 5);
    clear_area(rmon->window,0,0,width,height);
    draw_frame(rmon);
    draw_percent(rmon);
    draw_sample(rmon);
  }
}

/********** scroll **********/
static void scroll_graph(rmon)
Amon *rmon;
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
	h =  PeN * rmon ->sh;
	x2 = MarginLeft;
	y2 = MarginTop;
	x3 = MarginLeft + w;
	y3 = MarginTop;
	w1 = scroll * rmon -> sw;
	h1 = PeN * rmon-> sh;

	transfer(rmon->window,x1,y1,w,h,x2,y2);
        clear_area(rmon->window,x3,y3,w1,h1);
        rmon -> count = rmon -> cycle - scroll;
	
    }
}

/********** draw_data **********/
static void draw_data(rmon,data)
Amon *rmon;
unsigned short *data;
{
   unsigned short idle;
   unsigned short gc;
   unsigned short send;
   unsigned short rec;
   unsigned short comp;

   comp=*(data+0)*PeN*rmon->sh/100;
   send=*(data+1)*PeN*rmon->sh/100;
   rec =*(data+2)*PeN*rmon->sh/100;
   gc  =*(data+3)*PeN*rmon->sh/100;
   idle=PeN*rmon->sh-gc-send-rec-comp;
   if(gc+send+rec+comp > PeN*rmon->sh) idle=0;

   draw_one(rmon,4,comp,draw_one(rmon,3,send,draw_one(rmon,2,rec,draw_one(rmon,1,gc,draw_one(rmon,0,idle,MarginTop)))));

   flush(rmon->window);
}

int draw_one(rmon,color,h,y)
Amon *rmon;
int color;
unsigned int h;
int y;
{
   unsigned int w;
   int x;

   w=rmon->sw;
   x=MarginLeft+w*rmon->count;

   set_color(rmon->window,*(rmon->color+color));
   draw_filled_rectangle(rmon->window,x,y,w,h,1,NULL,pSet);
   set_color(rmon->window,"black");
   draw_line(rmon->window,x+w-1,y,x+w-1,y+h,1,pSolid,pSet);

   return(y+h);
}

/********** get_init_data **********/
static void get_init_data(rInit,wInfo,rmon)
AInit *rInit;
InitInfo *wInfo;
Amon *rmon;
{
  
    unsigned int GraphHeight;
    unsigned int GraphWidth;
    int min_width;
    int min_height;

    min_width = MarginLeft + MarginRight + Interval +
      (rInit-> cycle+1) * 3;
    min_height = MarginTop + MarginDown + PeN * 3;

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

    rmon->sw = (wInfo->width -(MarginLeft +Interval+MarginRight)) 
               / (rInit->cycle +1) ;
    rmon->sh = (wInfo->height-(MarginTop + MarginDown)) / PeN;

    GraphWidth = rInit->cycle * rmon->sw;
    GraphHeight = PeN * rmon->sh;

    rmon -> sx = MarginLeft + GraphWidth + Interval;
    rmon -> sy = MarginTop + GraphHeight + MarginDown -
	(SampleMarginDown + rmon ->sh * 5);

    rmon -> count = 0;
    rmon -> cycle = rInit -> cycle;
    rmon -> displayType = rInit -> displayType;
    rmon -> width = wInfo ->width;
    rmon -> height = wInfo -> height;

    if(rInit -> scroll < 1 ){
	rmon -> scroll = 1;
    }else{
	rmon -> scroll = rInit -> scroll;
    }
}

/********** init_draw **********/
static void init_draw(rmon)
Amon *rmon;
{
    activateSWin(rmon->window);
    draw_frame(rmon);
    draw_percent(rmon);  /*  !!!!  */
    draw_sample(rmon);
}

/**********  draw_frame **********/
static void draw_frame(rmon)
Amon *rmon;
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

  checker[0]=0x44;
  checker[1]=0x44;
  checker[2]=0x11;
  checker[3]=0x11;
  checker[4]=0x44;
  checker[5]=0x44;
  checker[6]=0x11;
  checker[7]=0x11;
  checker[8]=0x44;
  checker[9]=0x44;
  checker[10]=0x11;
  checker[11]=0x11;
  checker[12]=0x44;
  checker[13]=0x44;
  checker[14]=0x11;
  checker[15]=0x11;
  checker[16]=0x44;
  checker[17]=0x44;
  checker[18]=0x11;
  checker[19]=0x11;
  checker[20]=0x44;
  checker[21]=0x44;
  checker[22]=0x11;
  checker[23]=0x11;
  checker[24]=0x44;
  checker[25]=0x44;
  checker[26]=0x11;
  checker[27]=0x11;
  checker[28]=0x44;
  checker[29]=0x44;
  checker[30]=0x11;
  checker[31]=0x11;

  win = rmon->window;

  width = rmon->width;
  height = rmon->height;

  InLeftTopX = FrameWidth;
  InLeftTopY = FrameWidth;
  InLeftDownX = FrameWidth;
  InLeftDownY = height - FrameWidth;
  InRightTopX = width - FrameWidth;
  InRightTopY = FrameWidth;

  set_color(rmon->window,"black");
  draw_filled_rectangle(win,0,0,width,height,1,checker,pSet);
  clear_area(win,FrameWidth, FrameWidth, width-FrameWidth*2,
	     height-FrameWidth*2);
  set_color(win,"white");
  draw_line(win,1,0,1,height,2,pSolid,pSet);
  draw_line(win,1,1,width,1,2,pSolid,pSet);
  set_color(win,"black");
  draw_line(win,width-1,1,width-1,height,2,pSolid,pSet);
  draw_line(win,width,height-1,1,height-1,2,pSolid,pSet);
  draw_line(win,InLeftTopX,InLeftTopY-1,InLeftDownX,InLeftDownY,2,pSolid,pSet);
  draw_line(win,InLeftTopX-1,InLeftTopY,InRightTopX,InRightTopY,2,pSolid,pSet);

  draw_filled_rectangle(rmon->window, MarginLeft-1, MarginTop-1,
		   rmon->sw*rmon->cycle+2, rmon->sh*PeN+2, 1, NULL, pSet);
  clear_area(rmon->window, MarginLeft, MarginTop,
		   rmon->sw*rmon->cycle, rmon->sh*PeN);
}

/********** draw_sample **********/
static void draw_sample(rmon)
Amon *rmon;
{
   SWin *win;
   int sx;
   int sy;
   unsigned int sw;
   unsigned int sh;

   win=rmon->window;
   sx =rmon->sx;
   sy =rmon->sy;
   sw =rmon->sw;
   sh =rmon->sh;

   draw_one_sample(win,sx,sy     ,sw,sh,*(rmon->color+0),"idle");
   draw_one_sample(win,sx,sy+1*sh,sw,sh,*(rmon->color+1),"GC");
   draw_one_sample(win,sx,sy+2*sh,sw,sh,*(rmon->color+2),"receive");
   draw_one_sample(win,sx,sy+3*sh,sw,sh,*(rmon->color+3),"send");
   draw_one_sample(win,sx,sy+4*sh,sw,sh,*(rmon->color+4),"compute");
}

/********** draw_one_sample **********/
static void draw_one_sample(win,sx,sy,sw,sh,color,name)
SWin *win;
int sx;
int sy;
unsigned int sw;
unsigned int sh;
char *color;
char *name;
{
   unsigned int charW;
   unsigned int charH;

   set_color(win,color);
   draw_filled_rectangle(win,sx,sy,sw,sh,1,NULL,pSet);
   set_font(win,"a14");
   get_font_size(win,&charW,&charH);
   set_color(win,"black");
   draw_string(win,sx+sw+5,sy+sh/2-charH/2,name,"r14");
}

/********** draw_percent **********/
static void draw_percent(rmon)
Amon *rmon;
{
    SWin *win;
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

    win=rmon->window;
    get_font_size(win,&charW,&charH);
    drawHeight = charH / rmon->sh + 1;
    y = MarginTop + charH/2-2;
    x = MarginLeft -3;
    set_color(win,"black");

    draw_string(win,x-(charW*4),y+0*rmon->sh*PeN/5-charH,"100%","r14",pSet);
    draw_string(win,x-(charW*3),y+1*rmon->sh*PeN/5-charH,"80%" ,"r14",pSet);
    draw_string(win,x-(charW*3),y+2*rmon->sh*PeN/5-charH,"60%" ,"r14",pSet);
    draw_string(win,x-(charW*3),y+3*rmon->sh*PeN/5-charH,"40%" ,"r14",pSet);
    draw_string(win,x-(charW*3),y+4*rmon->sh*PeN/5-charH,"20%" ,"r14",pSet);
    draw_string(win,x-(charW*2),y+5*rmon->sh*PeN/5-charH,"0%"  ,"r14",pSet);

}

/********** set_palette **********/
static void set_palette(rmon)
Amon *rmon;
{
    *(rmon->color+0) = "#000000007d00";
    *(rmon->color+1) = "#c350fde8fde8";
    *(rmon->color+2) = "#0000f2307530";
    *(rmon->color+3) = "#ea60f6180000";
    *(rmon->color+4) = "#fde861a80000";

}

