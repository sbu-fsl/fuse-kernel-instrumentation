#!/bin/sh

echo "unmounting fusectl"
umount /sys/fs/fuse/connections
echo "removing fuse module"
rmmod fuse
echo "inserting fuse module"
insmod fuse.ko
echo "mounting fusectl"
mount -t fusectl fusectl /sys/fs/fuse/connections
echo "cleaning fuse kernel module"
make clean
