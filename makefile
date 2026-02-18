CC = gcc
CFLAGS = -Wall -Wextra -g 
OBJ = main.o fs_utils.o fs_core.o
EXEC = shell_fs

all: $(EXEC)

$(EXEC): $(OBJ)
		$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c 
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		rm -f $(OBJ) $(EXEC) filesystem.img