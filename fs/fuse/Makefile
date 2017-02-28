#
# Makefile for the FUSE filesystem.
#

obj-$(CONFIG_FUSE_FS) += fuse.o
obj-$(CONFIG_CUSE) += cuse.o

fuse-objs := dev.o dir.o file.o inode.o control.o

all: fusefs

fusefs:
	make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=/usr/src/linux-stable-fuse/fs/fuse/ modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=/usr/src/linux-stable-fuse/fs/fuse/ clean
