# Define the directories for source files, header files, object files, and output
SRCDIR := src
INCDIR := include
OBJDIR := obj
BINDIR := bin

# Define the compiler and compiler flags
CC := gcc
CFLAGS := -Wall -I$(INCDIR)

# Define the target executable
TARGET := $(BINDIR)/compiler

# List all your source files (assuming they all end with .c)
SRCS := $(wildcard $(SRCDIR)/*.c)

# Generate a list of object files based on the source files
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Specify the location of header files
VPATH := $(INCDIR)

# The default target is the executable
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile each source file into an object file in the OBJDIR
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure the output directories exist before compiling
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Clean up object files and the executable
clean:
	rm -rf $(OBJDIR) $(TARGET)