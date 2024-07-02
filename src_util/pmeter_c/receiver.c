#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* server program */ 
/* for TCP/IP */
#include "rmon20.h"
#include "amon.h" 
#include "pmeter20.h"
#include "prmon.h"
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#ifdef ME
#include <sys/netinet/in.h>
#else
#include <netinet/in.h>
#endif

/* for TCP/IP */
#define LINEMAX 5000

void calc_posi(string,posi_x,posi_y)
char	*string;
int	*posi_x, *posi_y;
{
  int		i, j;
  char	x[12], y[12];

  if(*string == '-') {
/*    for(i = 1;i < strlen(string); i++) {
      if(*(string+i) == '-') {
	for(j = i+1;j < strlen(string); j++) {
	  y[j-i-1] = *(string+j);
	}
	*posi_y = 1000 - atoi(y);
	i = j;
      }
      else if(*(string+i) == '+') {
	for(j = i+1;j < strlen(string); j++)
	  y[j-i-1] = *(string+j);
	*posi_y = atoi(y);
	i = j;
      }
      else
	x[i-1] = *(string+i);
    }
    *posi_x = 1000 - atoi(x); */
    *posi_x = 0;
    *posi_y = 0;
  }
  else if(*string == '+') {
    for(i = 1;i < strlen(string); i++) {
      if(*(string+i) == '-') {
	for(j = i+1;j < strlen(string); j++)
	  y[j-i-1] = *(string+j);
	*posi_y = 1000 - atoi(y);
	*posi_y = 0;
	i = j;
      }
      else if(*(string+i) == '+') {
	for(j = i+1;j < strlen(string); j++)
	  y[j-i-1] = *(string+j);
	*posi_y = atoi(y);
	i = j;
      }
      else
	x[i-1] = *(string+i);
    }
    *posi_x = atoi(x);
  }
  else {
    *posi_x = 0;
    *posi_y = 0;
  }
}

main(argc,argv)
int argc;
char *argv[];
{
  int	j, k, posi_x=0, posi_y=0, width=620, height=550;
  char	w[12], h[12];
  int	scroll_type = 2, background = 0;
  char	*half = "half", *one = "one", *quarter = "quarter";
  char    *vex = "convex", *cave = "concave", *plane = "plane";
  int     graph_type = 2;
  int c;
  FILE *fp;
  int fromlen;
  register int i, s, ns;
  struct sockaddr_in saun, fsaun;
 
       Amon *amon , *create_amonitor();
       AInit ainit;              

       Pmeter *pm , *create_pmeter();  
       PInit pinit;              

       Rmon *rmon , *create_rmonitor();
       RInit rinit;              

       PRmon *prmon , *create_prmon();
       PRInit prinit;              

    unsigned short int inline[LINEMAX], *rectop;
	
    int xj ;
    unsigned int sum;

    int  sw, sh, cycle ,position, pe;
    unsigned short int type , interval, item, color;
    int numbyte1, numbyte2, numbyte3, numbyte4 ;
    unsigned short int xpe, ype, cluster,node;
    int  pos , length;

    int maxbyte;
 
	for(j = 1;j < argc; ++j) {
	  if(*argv[j] == '-') {
	    switch(*(argv[j]+1)) {
	    case 's':
	      if(argv[j+1] == NULL) {
		printf("After '-s',type [one, half, quarter]\n");
		exit(0);
	      }
	      else if(strcmp(argv[j+1],half) == 0) {
		scroll_type = 2;
/*		printf("scroll_type=1/%d\n",scroll_type); */
		++j;
	      }
	      else if(strcmp(argv[j+1],quarter) == 0) {
		scroll_type = 4;
/*		printf("scroll_type=1/%d\n",scroll_type); */
		++j;
	      }
	      else if(strcmp(argv[j+1],one) == 0) {
		scroll_type = 1;
/*		printf("scroll_type=%d/cycle\n",scroll_type); */
		++j;
	      }
	      else {
		printf("After '-s',type [one, half, quarter]\n");
		exit(0);
	      }
	      break;
	    case 'g':
	      if(argv[j+1] == NULL) {
		j++;
	      }
	      else if(*argv[j+1] == '-' || *argv[j+1] == '+') {
		calc_posi(argv[j+1],&posi_x,&posi_y);
		j++;
	      }
	      else if(isdigit(*argv[j+1]) != 0) {
		for(i = 0;i < strlen(argv[j+1]); i++) {
		  if(*(argv[j+1]+i) == 'x') {
		    for(k = i+1;k < strlen(argv[j+1]); k++) {
		      if(*(argv[j+1]+k) == ' ')
			break;
		      if(*(argv[j+1]+k) == '-' || *(argv[j+1]+k) == '+') {
			calc_posi(argv[j+1]+k,&posi_x,&posi_y);
			break;
		      }
		      h[k-i-1] = *(argv[j+1]+k);
		    }
		    height = atoi(h);
		    i = k;
		    break;
		  }
		  else
		    w[i] = *(argv[j+1]+i);
		}
		width = atoi(w);
		j++;
	      }
	      else
		printf("After '-g',type\n");
/*	      printf("width = %d height = %d\n",width,height);
	      printf("posi_x = %d\n",posi_x);
	      printf("posi_y = %d\n",posi_y); */
	      break;
	    case 'b':
	      background = 1;
	      if(argv[j+1] == NULL) {
		graph_type = 0;
		++j;
	      } 
	      if(strcmp(argv[j+1],vex) == 0) {
		graph_type = 0;
		++j;
	      }
	      else if(strcmp(argv[j+1],cave) == 0) {
		graph_type = 1;
		++j;
	      }
	      else if(strcmp(argv[j+1],plane) == 0) {
		background = 0;
		++j;
	      }
	      else {
		graph_type = 0;
	      }
	      break;
	    default:
	      printf("Usage: pg-rmon [-b] [convex, concave, plane] [-s] [one, quater, half] ...\n");
	      exit(0);
	      break;
	    }
	  }
          else {
	    printf("Usage: pg-rmon [-b] [convex, concave, plane] [-s] [one, quarter, half] ...\n");
	    exit(0);
	  }
	}

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		perror("server: socket");
	exit(1);
	}

	saun.sin_family = AF_INET;
	saun.sin_port=4000;
	saun.sin_addr.s_addr=INADDR_ANY;

	if (bind(s, &saun, sizeof(saun)) < 0) {
		perror("server: bind");
	exit(1);
	}
	if (listen(s, 5) < 0){
		perror("server: listen");
	exit(1);
	}

      for(;;){
	if ((ns = accept(s, 0, 0)) < 0){
		perror("server:accept");
	exit(1);
	}

	fp = fdopen(ns, "r");
      
        if ( (numbyte3 = read(ns, inline, LINEMAX) ) <= 0){
          exit(1);
	}
          
        if ( inline[0] != 3 ){
           exit(1);
	}

	type = inline[1] ; 
	
        if ( (numbyte1 = read(ns, inline, LINEMAX) ) <= 0){
          exit(1);
	}

	position = 0;
        
        if ( inline[0] != 1 ){
           exit(1);
	 }
   
        if(type == 4)
	  {
	   cluster = inline[position+1] ;
           xpe = inline[position+2] ;
	   ype = inline[position+3] ;
           node = inline[position+4] ;
	   pe =  cluster * node;
	   interval = inline[position+5];
	   item = inline[position+6];
	   color = inline[position+7]; }
	else{
	   pe = inline[position+1] ;
           xpe = inline[position+2] ;
           ype = inline[position+3] ;
	   interval = inline[position+4];
	   item = inline[position+5];
	   color = inline[position+6];}
	  
	    
        if (type == 2)
	  {
             resetAinit(&ainit);
             ainit.x = 0;
             ainit.y = 0;
             ainit.cycle = 30;
	     if(scroll_type == 1){
	       scroll_type = 30;}
             ainit.scroll = scroll_type;
             ainit.title = " ParaGraph Runtime Monitor (Average)";
             ainit.width = 620;
	     ainit.height = 250;
             
             amon = create_amonitor( ainit );
	   }
	else{
	  if (type == 1){

             resetPinit(&pinit);
             pinit.x = posi_x;
             pinit.y = posi_y;
             pinit.title = " ParaGraph Performance Meter";
             pinit.width = width;
	     pinit.height = height;
             
             pm = create_pmeter(pe,xpe,ype, pinit);}

          else{
	    if (type == 0){
             resetRinit(&rinit);

	     rinit.x = posi_x;
             rinit.y = posi_y;
             if(scroll_type == 1){
	       scroll_type = 30;}
             rinit.scroll = scroll_type;
             rinit.cycle = 30;
             if(background == 1){
	       rinit.type = black;}
             rinit.title = " ParaGraph Runtime Monitor";
             rinit.width = width;
             rinit.height = height;
	     if(graph_type == 0)
	       rinit.uneven = convex;
	     else if(graph_type == 1)
	       rinit.uneven = concave;
	     
             rmon = create_rmonitor( pe, rinit ); }
          else{
             resetPRinit(&prinit);

	     prinit.x = posi_x;
             prinit.y = posi_y;
             if(scroll_type == 1){
	       scroll_type = 10;}
             prinit.scroll = scroll_type;
             prinit.cycle = 10;
             if(background == 1){
	       prinit.type = black;}
             prinit.title = " ParaGraph PIM Monitor";
             prinit.width = width;
             prinit.height = height;
	     if(graph_type == 0)
	       prinit.uneven = convex;
	     else if(graph_type == 1)
	       prinit.uneven = concave;
	     
             prmon = create_prmon( cluster, xpe, ype, node , prinit ); 
	   }
	  }}

       while( ( numbyte2 = read(ns, inline, 2)) > 0 ){
	    
       if ( inline[0] == 2 ){

       rectop = inline;

       if (type == 2){
       	  maxbyte = 8 ; }
       else{
	  maxbyte = (pe * 5) *2;
       }

       if ( ( numbyte4 = read_stream(ns, rectop, maxbyte)) > 0 ){
       if (type == 2){
       	  draw_amonitor(amon,&inline[0]); }
       else{
	if (type == 1){
	  draw_pmeter(pm,&inline[0]);}
        else{
         if (type == 0){
	    draw_rmonitor(rmon,&inline[0]);}
         else{
	    draw_prmon(prmon,&inline[0]);}
      }}}}}

       if ( numbyte2 == 0) {
         if (type == 2){
       	    kill_amonitor(amon); }
          else{
	    if (type == 1){
	     kill_pmeter(pm); }
            else{
	      if (type == 0){
	        kill_rmonitor(rmon); }
              else{
                kill_prmon(prmon); }}}}
        else {
        exit(1); }
	  
	  }}

int read_stream(stream, buffer, linemax)
     int stream, linemax;
     unsigned short int *buffer;
{
  int numbyte, totalbyte = 0;

  while((( numbyte=read(stream, buffer, linemax)) > 0 )&&
	(linemax -=numbyte) > 0 ) {
    totalbyte += numbyte;
    buffer += numbyte/sizeof(unsigned short int);
  }
 if (linemax == 0){
   return numbyte;
 }
  else{
    return -1;
  }
}
