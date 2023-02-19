CFLAGS+=-pthread
LDFLAGS+=-pthread -lstdc++ -lstdc++

all:mysem

mysem:main.o mysem.o
	gcc $^ -g -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o mysem