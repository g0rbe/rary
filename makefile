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

build: utils.c utils.h rary.h
	@gcc -c -Wall -Werror -fpic -o utils.o utils.c
	@gcc -shared -o library.so utils.o

install: createpath build
	@mv library.so /usr/lib64/library.so
	@chmod 0755 /usr/lib64/library.so
	@cp utils.h /usr/include/rary/utils.h
	@cp rary.h /usr/include/rary/rary.h

clean: utils.o
	@rm utils.o


remove:
	rm -rf /usr/include/g0rbe