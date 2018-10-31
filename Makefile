LDLIBS=-lm /usr/local/lib/libwiringPi.so
CFLAGS=-Os -Wall
DOCCO=docco

olympic: olympic.c phone.c
	$(CC) $(CFLAGS) olympic.c phone.c $(LDLIBS) -o $@

docs: olympic.c phone.c
	$(DOCCO) olympic.c phone.c 

clean:
	rm -f olympic
