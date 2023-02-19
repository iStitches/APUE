CFLAGS+=-pthread
LDFLAGS+=-pthread

app:mytbf

mytbf:main.o mytbf.o
	gcc $^ -g -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o mytbf