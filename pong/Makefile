#OBJS specifies which files to compile as part of the project
OBJS = LTimer.cpp LTexture.cpp noballpong.cpp
OBJSO = LTimer.o LTexture.o noballpong.o

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

#OBJ_NAME specifies the name of our exectuable
TARGET = pong

#This is the target that compiles our executable
all : objects main

objects: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) -c
	
main : 
	$(CC) $(OBJSO) $(LINKER_FLAGS) -o $(TARGET)
	
clean:
	rm -rf *.o pong
