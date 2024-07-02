#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
/* server program */ 
/* for TCP/IP */
#include "rmon20.h"
#include <stdio.h>
#include <sys/socket.h>
#ifdef ME
#include <sys/netinet/in.h>
#else
#include <netinet/in.h>
#endif

/* for TCP/IP */
#define LINEMAX 5000

main(argc,argv)
int argc;
char *argv[];
{
	int	j, scroll_type = 2 ,background = 0;
	char	*half = "half", *one = "one", *quarter = "quarter";
	char    *vex = "convex", *cave = "concave", *plane = "plane";
	int     graph_type = 2;
	int c;
	FILE *fp;
	int fromlen;
	register int i, s, ns;
	struct sockaddr_in saun, fsaun;
 
       Rmon *rmon , *create_rmonitor();
       RInit rinit;              

    unsigned short int inline[LINEMAX], *rectop;
	
    int xj ;
    unsigned int sum;

    int  sw, sh, cycle ,position, pe;
    unsigned short int type , interval, item, color;
    int numbyte1, numbyte2, numbyte3, numbyte4 ;
    unsigned short int xpe, ype;
    int  pos , length;

    int maxbyte;
 
	for(j = 1;j < argc; ++j) {
	  if(*argv[j] == '-') {
	    switch(*(argv[j]+1)) {
	    case 's':
/*	      printf("*(argv[%d]+1)=%s\n",j,argv[j]+1);
	      printf("*(argv[%d+1])=%s\n",j,argv[j+1]); */
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
	      printf("Usage: pg-hmon [-b] [convex, concave, plane] [-s] [one, quater, half] ...\n");
	      exit(0);
	      break;
	    }
	  }
          else {
	    printf("Usage: pg-hmon [-b] [convex, concave, plane] [-s] [one, quarter, half] ...\n");
	    exit(0);
	  }
	}

       if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		perror("server: socket");
	exit(1);
	}

	saun.sin_family = AF_INET;
	saun.sin_port=4001;
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

	pe = inline[position+1] ;
        xpe = inline[position+2] ;
        ype = inline[position+3] ;
	interval = inline[position+4];
	item = inline[position+5];
	color = inline[position+6];
	    
             resetRinit(&rinit);

	     rinit.x = 0;
             rinit.y = 0;
             rinit.scroll = scroll_type;
             rinit.cycle = 10;
             if(background == 1){
	       rinit.type = black;}
             rinit.title = " ParaGraph Heap Monitor";
             rinit.width = 320;
             rinit.height = 550;
	     if(graph_type == 0)
	       rinit.uneven = convex;
	     else if(graph_type == 1)
	       rinit.uneven = concave;
	     
             rmon = create_rmonitor( pe, rinit ); 

	 while( ( numbyte2 = read(ns, inline, 2)) > 0 ){
	    
         if ( inline[0] != 2 && inline[0] != 4) {
            exit(1);
	  }
	   
       if ( inline[0] == 2 ){

       rectop = inline;

       if (type == 2){
       	  maxbyte = 8 ; }
       else{
	  maxbyte = (pe * 5) *2;
       }

       if ( ( numbyte4 = read_stream(ns, rectop, maxbyte)) > 0 ){
	  draw_rmonitor(rmon,&inline[0]);}
     }}

       if ( numbyte2 == 0) {
          kill_rmonitor(rmon); }
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
