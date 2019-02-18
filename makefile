all: list parser main

list: LinkedList.o liblist.so

LinkedList.o: src/LinkedListAPI.c include/LinkedListAPI.h
	gcc -Wall -std=c11 -c -fpic -Iinclude/../include src/LinkedListAPI.c -o bin/LinkedListAPI.o
	
liblist.so: bin/LinkedListAPI.o
	gcc -shared -o bin/liblist.so bin/LinkedListAPI.o

parser: calendarParser.o HelperFunctions.o libcal.so

calendarParser.o: src/CalendarParser.c include/CalendarParser.h
	gcc -Wall -std=c11 -c -fpic -Iinclude/../include src/CalendarParser.c -o bin/CalendarParser.o

HelperFunctions.o: src/HelperFunctions.c include/HelperFunctions.h
	gcc -Wall -std=c11 -c -fpic -Iinclude/../include src/HelperFunctions.c -o bin/HelperFunctions.o

libcal.so:	bin/CalendarParser.o
	gcc -shared -o bin/libcal.so bin/CalendarParser.o

main: src/main.c bin/LinkedListAPI.o bin/CalendarParser.o bin/HelperFunctions.o
	gcc -Iinclude/../include -Wall -std=c11 -c src/main.c -o bin/main.o
	gcc bin/LinkedListAPI.o bin/CalendarParser.o bin/HelperFunctions.o bin/main.o -o main

clean:
	rm -rf bin/*.o bin/*.so
