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

# Build utils.o
utils.o: utils.c utils.h
	@gcc -c -Wall -Werror -fpic -o utils.o utils.c

# Build systemd.o
systemd.o: systemd.c systemd.h
	@gcc -c -Wall -Werror -fpic -o systemd.o systemd.c

# Build library.so
library.so: utils.o systemd.o
	@gcc -shared -o library.so utils.o systemd.o

# Install
install: createpath library.so
	@cp library.so /usr/lib64/library.so
	@chmod 0755 /usr/lib64/library.so
	@cp utils.h /usr/include/rary/utils.h
	@cp rary.h /usr/include/rary/rary.h
	@cp systemd.h /usr/include/rary/systemd.h

# Remove unused binaries 
clean: utils.o
	@rm utils.o
	@rm systemd.o
	@rm library.so

# Remove rary
remove:
	@rm -rf /usr/include/rary
	@rm -f /usr/lib64/library.so