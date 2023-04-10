CC = gcc
CPPFLAGS =

.PHONY: test clean

seq.o: seq.c
	gcc -c -g -Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2 $^ 

memory_tests.o: memory_tests.c
	gcc -c -g -Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2 $^ 

libseq.so: seq.o memory_tests.o
	gcc -g -shared -o $@ $^

seq_example.o: seq_example.c
	gcc -c -g -Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2 $^ 

test: seq_example.o libseq.so
	gcc -L. -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup -o $@ $< -lseq

clean:
	rm *.o *.so
