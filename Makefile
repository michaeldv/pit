#!/usr/bin/make

# Prefix directory where pit binary gets installed to.
PREFIX = /usr/local

# The toplevel directory of the source tree.
SRCDIR = ./src

# The directory into which object code files should be written.
OBJDIR = ./obj

# The directory into which executable file should be written.
BINDIR = ./bin

# C Compiler and options for use in building executable.
CC = gcc -g -Os -Wall

SRC = \
	$(SRCDIR)/action.c \
	$(SRCDIR)/args.c \
	$(SRCDIR)/db.c \
	$(SRCDIR)/help.c \
	$(SRCDIR)/note.c \
	$(SRCDIR)/format.c \
	$(SRCDIR)/pit.c \
	$(SRCDIR)/project.c \
	$(SRCDIR)/table.c \
	$(SRCDIR)/task.c \
	$(SRCDIR)/util.c

OBJ = \
	$(OBJDIR)/action.o \
	$(OBJDIR)/args.o \
	$(OBJDIR)/db.o \
	$(OBJDIR)/help.o \
	$(OBJDIR)/note.o \
	$(OBJDIR)/format.o \
	$(OBJDIR)/pit.o \
	$(OBJDIR)/project.o \
	$(OBJDIR)/table.o \
	$(OBJDIR)/task.o \
	$(OBJDIR)/util.o

APP = pit

all: $(OBJDIR) $(APP)

$(OBJDIR):
	-mkdir $(OBJDIR)
	-mkdir $(BINDIR)

$(APP): $(OBJ)
	$(CC) -o $(BINDIR)/$(APP) $(OBJ)

$(OBJDIR)/action.o: $(SRCDIR)/action.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/action.o -c $(SRCDIR)/action.c

$(OBJDIR)/args.o: $(SRCDIR)/args.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/args.o -c $(SRCDIR)/args.c

$(OBJDIR)/db.o: $(SRCDIR)/db.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/db.o -c $(SRCDIR)/db.c

$(OBJDIR)/help.o: $(SRCDIR)/help.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/help.o -c $(SRCDIR)/help.c

$(OBJDIR)/note.o: $(SRCDIR)/note.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/note.o -c $(SRCDIR)/note.c

$(OBJDIR)/format.o: $(SRCDIR)/format.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/format.o -c $(SRCDIR)/format.c

$(OBJDIR)/pit.o: $(SRCDIR)/pit.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/pit.o -c $(SRCDIR)/pit.c

$(OBJDIR)/project.o: $(SRCDIR)/project.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/project.o -c $(SRCDIR)/project.c

$(OBJDIR)/table.o: $(SRCDIR)/table.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/table.o -c $(SRCDIR)/table.c

$(OBJDIR)/task.o: $(SRCDIR)/task.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/task.o -c $(SRCDIR)/task.c

$(OBJDIR)/util.o: $(SRCDIR)/util.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/util.o -c $(SRCDIR)/util.c

install:
	cp $(BINDIR)/$(APP) $(PREFIX)/bin/$(APP)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/$(APP)
	rmdir $(OBJDIR)
	rmdir $(BINDIR)

.PHONY : test
test:
	./test/pit_test.rb
	rm -f ./test/test.pitfile
