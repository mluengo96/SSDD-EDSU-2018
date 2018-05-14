CFLAGS=-Wall

all: test_editor test_editor_avanz

edsu_comun.o: edsu_comun.h

comun.o: comun.h

editor.o: comun.h editor.h edsu_comun.h

test_editor.o: editor.h

test_editor: test_editor.o editor.o edsu_comun.o comun.o
	$(CC) -o $@ test_editor.o editor.o edsu_comun.o comun.o -lpthread

test_editor_avanz.o: editor.h

test_editor_avanz: test_editor_avanz.o editor.o edsu_comun.o comun.o
	$(CC) -o $@ test_editor_avanz.o editor.o edsu_comun.o comun.o -lpthread

clean:
	rm *.o test_editor test_editor_avanz
