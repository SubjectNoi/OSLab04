all : liblock libcounter liblist libhash

liblock : lock.c
	gcc lock.c -fPIC -shared -o liblock.so

libcounter : counter.c
	gcc counter.c -L. -llock -fPIC -shared -o libcounter.so

liblist : list.c
	gcc list.c -L. -llock -fPIC -shared -o liblist.so 

libhash : hash.c
	gcc hash.c -L. -llock -fPIC -shared -o libhash.so 
