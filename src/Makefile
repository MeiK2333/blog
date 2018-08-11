link = g++ -o
gcc11 = g++ -std=c++11 -c

lib = config logger utils request response master work event
lib.o = $(foreach file,$(lib),$(file).o)
lib.cpp = $(foreach file,$(lib),$(file).cpp)

blog: main.cpp $(lib.cpp)
	$(gcc11) main.cpp
	$(gcc11) $(lib.cpp)
	$(link) blog main.o $(lib.o)
	rm *.o
	
clean:
	rm -rf *.o