#OBJS specifies which files to compile as part of the project
OBJS = main.c ./src/add.c ./src/file_helpers.c ./src/git_helpers.c ./src/show.c ./src/complete.c ./src/note.c

#CC specifies our compiler
CC = gcc

COMPILER_FLAGS = -Wall

LINKER_FLAGS = -I./ 

#OBJ_NAME specifies the name of our executable
OBJ_NAME = git-note

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
