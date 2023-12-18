# Define the directories for source files, header files, object files, and output
SRCDIR := src
INCDIR := include
OBJDIR := obj
BINDIR := bin
GENDIR := out_gen

# Define the compiler and compiler flags
CC := gcc
CFLAGS := -Wall -g -I$(INCDIR)

# Define the target executable
TARGET := $(BINDIR)/compiler

# List all your source files (assuming they all end with .c)
SRCS := $(wildcard $(SRCDIR)/alex/*.c) \
		$(wildcard $(SRCDIR)/sintactic/*.c) \
		$(wildcard $(SRCDIR)/symbols/*.c) \
		$(wildcard $(SRCDIR)/atip/*.c) \
		$(wildcard $(SRCDIR)/gen/*.c) \

# Generate a list of object files based on the source files
OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%, $(patsubst %.c,%.o,$(SRCS)))

# Specify the location of header files
VPATH := $(INCDIR)

# The default target is the executable
all: clean $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile each source file into an object file in the OBJDIR
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure the output directories exist before compiling
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Clean up object files and the executable
clean:
	rm -rf $(OBJDIR) $(TARGET)
	rm -rf $(GENDIR)/out_1.c

run-debug:
	gdb ./bin/compiler

run:
	./bin/compiler
	touch $(GENDIR)/out_1.c
	make run1

# Compile and run the out_1.c file
run1: $(GENDIR)/out_1.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/out_1 $<
	$(BINDIR)/out_1
