#OBJS specifies which files to compile as part of the project
OBJS = main.c add.c file_helpers.c git_helpers.c

#CC specifies our compiler
CC = gcc

# COMPILER_FLAGS = -w -g

LINKER_FLAGS = -I./ 

#OBJ_NAME specifies the name of our executable
OBJ_NAME = git-note

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
