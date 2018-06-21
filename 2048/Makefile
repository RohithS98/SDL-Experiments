CFLAGS = -c -g -w
LFLAGS = -w
SDLFLAGS = -lSDL2 -lSDL2_ttf

all : maintext.o LText.o gameboard.o main.o
	g++ maintext.o gameboard.o $(LFLAGS) -o 2048text
	g++ main.o LText.o gameboard.o $(SDLFLAGS) $(LFLAGS) -o 2048 
	
maintext.o : maintext.cpp gameboard.h
	g++ maintext.cpp $(CFLAGS)
	
main.o : main.cpp gameboard.h
	g++ main.cpp $(CFLAGS)

Ltext.o : LText.cpp Ltext.h
	g++ LText.xpp $(CFLAGS)

gameboard.o : gameboard.cpp gameboard.h
	g++ gameboard.cpp $(CFLAGS)
	
clean:
	rm -rf *.o 2048text 2048
