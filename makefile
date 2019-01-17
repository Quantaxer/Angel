all: list parser main

list: src/LinkedListAPI.c include/LinkedListAPI.h
	gcc -Wall -std=c11 -c -I../include src/LinkedListAPI.c -o bin/LinkedListAPI.o
	
parser: src/CalendarParser.c include/CalendarParser.h
	gcc -Wall -std=c11 -c -I../include src/CalendarParser.c -o bin/CalendarParser.o
	
main: src/main.c bin/LinkedListAPI.o bin/CalendarParser.o
	gcc -I../include -Wall -std=c11 -c src/main.c -o bin/main.o
	gcc bin/LinkedListAPI.o bin/CalendarParser.o bin/main.o -o main

clean:
	rm *.o core
