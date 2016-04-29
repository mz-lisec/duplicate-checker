all:
	g++ dupcheck.cc -o dupcheck -std=c++11 -O2

clean:
	rm dupcheck