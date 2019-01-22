all: list parser main

list: src/LinkedListAPI.c include/LinkedListAPI.h
	gcc -Wall -std=c11 -c -Iinclude/../include src/LinkedListAPI.c -o bin/LinkedListAPI.o
	gcc -Wall -std=c11 -c -shared -Iinclude/../include src/LinkedListAPI.c -o bin/LinkedListAPI.so
	
parser: src/CalendarParser.c include/CalendarParser.h
	gcc -Wall -std=c11 -c -Iinclude/../include src/CalendarParser.c -o bin/CalendarParser.o
	gcc -Wall -std=c11 -c -shared -Iinclude/../include src/CalendarParser.c -o bin/CalendarParser.so
	
main: src/main.c bin/LinkedListAPI.o bin/CalendarParser.o
	gcc -Iinclude/../include -Wall -std=c11 -c src/main.c -o bin/main.o
	gcc bin/LinkedListAPI.o bin/CalendarParser.o bin/main.o -o main

clean:
	rm *.o core
