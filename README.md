# OSLab04
# $ cd [your working dir]
# $ gcc -o lock main.c counter.c [list.c hash.c] lock.c -lpthread
# $ ./lock x(0 < x <= 1024)[启用线程数，超过1024需在main.c中更改malloc部分]
# if use makefile
# $ cd [your working dir]
# $ make
# $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
# $ gcc main.c -L. -llock -llist -lcounter -lhash -o test -lpthread
# $ ./lock x(0 < x <= 1024)[启用线程数，超过1024需在main.c中更改malloc部分]（可能会遇见core dumped情况）
