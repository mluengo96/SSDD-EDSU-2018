CFLAGS=-Wall

all: test_subscriptor test_subscriptor_avanz

edsu_comun.o: edsu_comun.h

comun.o: comun.h

subscriptor.o: comun.h subscriptor.h edsu_comun.h

test_subscriptor.o: subscriptor.h

test_subscriptor: test_subscriptor.o subscriptor.o edsu_comun.o comun.o
	$(CC) -o $@ test_subscriptor.o subscriptor.o edsu_comun.o comun.o -lpthread

test_subscriptor_avanz.o: subscriptor.h

test_subscriptor_avanz: test_subscriptor_avanz.o subscriptor.o edsu_comun.o comun.o
	$(CC) -o $@ test_subscriptor_avanz.o subscriptor.o edsu_comun.o comun.o -lpthread

clean:
	rm *.o test_subscriptor test_subscriptor_avanz
