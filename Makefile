INCLUDES = -I/usr/java8_64/include/ \
		   -I./

all:
	$(MAKE) clean
	xlc -g realHTMLcall.c -o realHTMLcall $(INCLUDES)
	#xlc -G -o realHTMLjni.so realHTMLjni.o

java:
	@javac -cp /SAG/tom/v7042/lib/servlet-api.jar RealHTMLConnector.java

clean:
	@rm -f *.o
	@rm -f *.so
