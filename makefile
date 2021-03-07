SHELL=/bin/bash

# Check who is running the makefile
# Should be root
checkroot:
ifneq ($(shell whoami), root)
	@echo "Run as root!"
	@exit 1
endif

# Create the include path if not exist
createpath: checkroot
ifeq ($(shell [ -e /usr/include/rary ] && echo 1 || echo 0 ), 0)
	@echo "Creating /usr/include/rary..."
	@mkdir /usr/include/rary
endif
ifeq ($(shell [ -e bin ] && echo 1 || echo 0 ), 0)
	@echo "Creating bin..."
	@mkdir bin
endif

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

all: file.o string.o systemd.o utils.o
	@gcc -shared -o bin/library.so bin/file.o bin/string.o bin/systemd.o bin/utils.o


# Build the objects
build: createpath
	@gcc -c -Wall -Werror -fpic src/*.c
	@gcc -shared -o library.so *.o
	@mv *.o bin/
	@mv library.so bin/

# Install
install: build
	@cp bin/library.so /usr/lib/x86_64-linux-gnu
	@chmod 0755 /usr/lib/x86_64-linux-gnu/library.so
	@cp src/*.h /usr/include/rary/

# Remove unused binaries 
clean: 
	@rm -rf bin/
	
# Remove rary
remove:
	@rm -rf /usr/include/rary
	@rm -f /usr/lib/x86_64-linux-gnu/library.so

# Install man pages
install-man: checkroot
	@gzip man/*
	@cp man/* /usr/share/man/man3/
	@gzip -d man/*
