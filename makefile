all:
	g++ main.cpp xmllexer.cpp xmlparser.cpp -std=c++11 -licuuc -licuio -o xjconv
