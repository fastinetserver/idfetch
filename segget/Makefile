segget: main.o
	g++ -o segget main.o -ljson -lcurl
main.o: main.cpp pkg.cpp distfile.cpp segment.cpp
	g++ -c main.cpp pkg.cpp distfile.cpp segment.cpp
clean:
	@-rm segget *.o
	@-rm distfile.seg*