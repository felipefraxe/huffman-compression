# Compiler and flags
CC := clang
CFLAGS := -std=c23 -Wall -Wextra -Iinclude

# Directories
SRCDIR := src
BUILDDIR := build
BINDIR := bin
INCDIR := include

SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRC))

# Executables
COMPRESS := $(BINDIR)/compress
DECOMPRESS := $(BINDIR)/decompress

# Default target
all: $(COMPRESS) $(DECOMPRESS)

# Build compress
$(COMPRESS): $(BUILDDIR)/compress.o $(BUILDDIR)/bit_reader.o $(BUILDDIR)/bit_writer.o $(BUILDDIR)/huff.o | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

# Build decompress
$(DECOMPRESS): $(BUILDDIR)/decompress.o $(BUILDDIR)/bit_reader.o $(BUILDDIR)/bit_writer.o $(BUILDDIR)/huff.o | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

# Pattern rule for object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build and bin directories if they don't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

.PHONY: all clean