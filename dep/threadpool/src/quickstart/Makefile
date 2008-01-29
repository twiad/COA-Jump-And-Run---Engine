include threadpool.makefile

all : clean QuickStart.o QuickStart

clean :
	rm -f *.o
	rm -f QuickStart

QuickStart.o : QuickStart.cpp
	$(cpp_comp) $(all_include_dirs) -Wall -c $< -o $@

QuickStart : QuickStart.o
	$(cpp_comp)  -o QuickStart  $(boost_thread_lib) QuickStart.o
