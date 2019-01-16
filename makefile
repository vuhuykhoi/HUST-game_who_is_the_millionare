default: main

main:
	clear 
	gcc -w -g -o client User_ll.c message.c gamehelper.c client.c database.c -l sqlite3
	gcc -o server User_ll.c message.c gamehelper.c server.c database.c -l sqlite3

w:
	clear 
	gcc -Wall -g -o client User_ll.c message.c gamehelper.c client.c database.c -l sqlite3
	gcc -o server User_ll.c message.c gamehelper.c server.c database.c -l sqlite3

run: main
	./client 
debug: main
	gdb ./client	
server: main
	./server
s: server	
clean:
	rm -f client server
