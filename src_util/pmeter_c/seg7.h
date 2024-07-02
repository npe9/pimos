#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* 1991.1.20 */
#ifndef _Seg7_Included
#define _Seg7_Included

struct count_init_info {
    int     layout_mode;
    int     x;
    int     y;
    unsigned int width;
    unsigned int height;
    int length;
    int line_width;
    unsigned int margin;
};

struct count_indicator {
    int     layout_mode;
    SWin    *window;
    int     x;
    int     y;
    unsigned int width;
    unsigned int height;
    int length;
    int line_width;
    int count;
    int     x1;
    int     y1;
    unsigned int w1;
    unsigned int h1;
    unsigned int margin;
};

typedef struct count_init_info CInfo;
typedef struct count_indicator CountI;

extern void reshape_counter();
extern int countup();
extern int reset_counter();
extern int set_counter();
extern CountI *create_count_indicator();

#endif
