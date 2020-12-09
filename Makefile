LIBS = -lcurses

tetris:tetris.cpp
	g++ -o tetris tetris.cpp ${LIBS}
