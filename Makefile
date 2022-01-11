#OBJS specifies which files to compile as part of the project
OBJS = main.c 

#CC specifies our compiler
CC = gcc

# COMPILER_FLAGS = -w -g

# LINKER_FLAGS = -lSDL2 -I./ -I./Pieces/ -I./Board/

#OBJ_NAME specifies the name of our executable
OBJ_NAME = git-note

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)