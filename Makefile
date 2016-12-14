INCLUDES = -I/usr/java8_64/include/ \
		   -I./

all:
	$(MAKE) clean
	xlc -c -fpic realHTMLjni.c -o realHTMLjni.o $(INCLUDES)
	xlc -G -o realHTMLjni.so realHTMLjni.o

java:
	@javac Start.java

clean:
	@rm -f *.o
	@rm -f *.so
