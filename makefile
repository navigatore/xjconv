all:
	g++ main.cpp xmllexer.cpp xmlparser.cpp jsongenerator.cpp -std=c++11 -o xjconv -licuuc -licuio
