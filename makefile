SHELL=/bin/bash

O_FILES = bin/file.o bin/string.o bin/systemd.o bin/utils.o

# Check who is running the makefile
# Should be root
checkroot:
ifneq ($(shell whoami), root)
	@echo "Run as root!"
	@exit 1
endif

# Create the include path if not exist
createpath:
	@mkdir /usr/include/rary || true
	@mkdir bin/ || true

# Create file.o
file.o: src/file.c
	@gcc -c -Wall -Werror -fpic -o bin/file.o		src/file.c

# Create string.o
string.o: src/string.c
	@gcc -c -Wall -Werror -fpic -o bin/string.o		src/string.c

# Create systemd.o
systemd.o: src/systemd.c
	@gcc -c -Wall -Werror -fpic -o bin/systemd.o	src/systemd.c

# Create utils.o
utils.o: src/utils.c
	@gcc -c -Wall -Werror -fpic -o bin/utils.o		src/utils.c

# Create library.so
library.so: file.o string.o systemd.o utils.o
	@gcc -shared -o bin/library.so $(O_FILES)
# create library.a
library.a: file.o string.o systemd.o utils.o
	@ar rcs bin/library.a $(O_FILES)

# Build the objects
build: createpath library.so library.a

# Install
install: checkroot build man
	@echo "Install library.so..."
	@cp bin/library.so /usr/lib/x86_64-linux-gnu
	@chmod 0755 /usr/lib/x86_64-linux-gnu/library.so

	@echo "Install library.a..."
	@cp bin/library.a /usr/lib/x86_64-linux-gnu
	@chmod 0755 /usr/lib/x86_64-linux-gnu/library.a

	@echo "Installing header files..."
	@cp src/*.h /usr/include/rary/
	
	@echo "Removing object files..."
	@rm -rf bin/

# Remove unused binaries 
clean: 
	@rm -rf bin/
	
# Remove rary
remove:
	@rm -rf /usr/include/rary
	@rm -f /usr/lib/x86_64-linux-gnu/library.so
	@rm -f /usr/lib/x86_64-linux-gnu/library.a

# Install man pages
man: checkroot
	@gzip man/*
	@cp man/* /usr/share/man/man3/
	@gzip -d man/*
