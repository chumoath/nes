t:
	g++ -g -I include ./src/*.cpp ./main.cpp -o main -lsfml-graphics -lsfml-window -lsfml-system

# disassemble
#test: 
#	g++ -g -I include ./test/Disassemble.cpp ./src/*.cpp -o main

# cartidge
#test:
#	g++ -g -I include ./test/TestCartidge.cpp ./src/Cartidge.cpp -o main
