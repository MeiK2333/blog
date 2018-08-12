link = g++ -o
gcc11 = g++ -std=c++11 -c

lib = config logger utils request response master work event read_buffer read_buffer_tree
lib.o = $(foreach file,$(lib),$(file).o)
lib.cc = $(foreach file,$(lib),$(file).cc)

blog: main.cc $(lib.cc)
	$(gcc11) main.cc
	$(gcc11) $(lib.cc)
	$(link) blog main.o $(lib.o)
	rm *.o
	
clean:
	rm -rf *.o