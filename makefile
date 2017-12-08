make:
	gcc -c -fpic counter.c
	gcc -shared -o libcounter.so counter.o
	gcc -c -fpic list.c
	gcc -shared -o liblist.so list.o
	gcc -c -fpic hash.c
	gcc -shared -o libhash.so hash.o
	gcc -lcounter -llist -lhash -L. -o lock main.c lock.c libcounter.so liblist.so libhash.so -lpthread

