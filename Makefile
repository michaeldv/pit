#!/usr/bin/make
#

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
	$(SRCDIR)/pager.c \
	$(SRCDIR)/pit.c \
	$(SRCDIR)/project.c \
	$(SRCDIR)/table.c \
	$(SRCDIR)/task.c \
	$(SRCDIR)/util.c

OBJ = \
	$(OBJDIR)/action.o \
	$(OBJDIR)/args.o \
	$(OBJDIR)/db.o \
	$(OBJDIR)/pager.o \
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

$(OBJDIR)/pager.o: $(SRCDIR)/pager.c $(SRCDIR)/pit.h $(SRCDIR)/object.h $(SRCDIR)/table.h
	$(CC) -o $(OBJDIR)/pager.o -c $(SRCDIR)/pager.c

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

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/$(APP)
	rmdir $(OBJDIR)
	rmdir $(BINDIR)

