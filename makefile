CC = gcc
SRCDIR = src
OBJDIR = bin
CFLAGS = -Wall -Wextra -lSDL2
TARGET = bin/graphics.o\
		 bin/chip8.o


# Executable name
EXEC = Chip8

all: $(EXEC)

#compile tous les .o
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -g

#compile main
main.o : main.c
	$(CC) -o main.o -c main.c $(OPT)

$(EXEC): $(TARGET) $(OBJDIR)/main.o
	$(CC) $(TARGET) $(CFLAGS) $(OBJDIR)/main.o -o $(EXEC)

debug: $(TARGET) $(OBJDIR)/main.o
	$(CC) $(TARGET) $(CFLAGS) $(OBJDIR)/main.o -o $(EXEC)_debug -g

help:
	@echo make : compile le code de production
	@echo make clean : permet de supprimer les fichiers


clean:
	rm -f $(OBJDIR)/*.o
	rm -rf $(EXEC)
	rm -rf $(EXEC)_debug
