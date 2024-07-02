#(C)1992 Institute for New Generation Computer Technology
#	(Read COPYRIGHT for detailed information.)
#CFLAGS = -DME
CFLAGS = -DSPARC
#CFLAGS = -DSUN3


pg-rmon : receiver.o prmon.o pmeter20.o rmon20.o amon.o swin.o seg7.o
	cc -o pg-rmon -lX11 receiver.o prmon.o pmeter20.o rmon20.o amon.o swin.o seg7.o
pg-hmon : hmonitor.o rmon20.o swin.o seg7.o
	cc -o pg-hmon -lX11 hmonitor.o rmon20.o swin.o seg7.o 


clean : 
	/bin/rm -f *.o core pg-rmon pg-hmon

