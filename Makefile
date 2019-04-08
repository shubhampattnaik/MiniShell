SRC:=$(wildcard *.c)
OBJ:=$(patsubst %.c, %.o, $(SRC))
minishell.exe: $(OBJ)
	gcc -o $@ $(OBJ)
clean: 
	rm *.o *.exe
