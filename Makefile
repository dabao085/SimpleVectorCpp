TestVector : Vector.h TestVector.cc
	g++ -g -o TestVector TestVector.cc -std=c++11
clean:
	rm -f TestVector core.*	