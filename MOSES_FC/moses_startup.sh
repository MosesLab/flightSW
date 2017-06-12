#!/bin/bash
# 
# File:   moses_startup.sh
# Author: byrdie
#
# Created on Jun 24, 2015, 2:24:42 PM
#
# Script that configures all of the devices used by MOSES flight software and executes 
# flight software at boot-time.

# Import bash environment variables to construct the PATH variable correctly
source /root/.bashrc
source /root/.profile

#set serial ports to the correct irq
setserial /dev/ttyS2 irq 10
setserial /dev/ttyS3 irq 11

# Set PLX PCI bridge to the correct IRQ
setpci -v -s 00:02.0 INTERRUPT_LINE=09
echo 1 > /sys/bus/pci/devices/0000:00:02.0/remove
echo 1 > /sys/bus/pci/rescan

#load PLX PCI bridge drivers
PLX_SDK_DIR=/usr/src/plx/PlxSdk
export PLX_SDK_DIR

#test synclink
path="/dev/ttyUSB"
lastTry="5"
testSuccess="mgslutil v1.20"
for x in 0 1 2 3 4 5
do
        device=$path$x
        if [ -e $device ]
        then
                output=$(/usr/local/sbin/mgslutil $device rs422 | grep "mgslutil")

                if [ "$output" = "$testSuccess" ]
                then
                        echo "synclink device found on: "$device
                        break
                fi
        fi

        if [ $x = $lastTry ]
        then
                echo "Error: synclink not found"
                break
        fi

done

exec Plx_load 9056 &

sleep 1

exec moses_fc &

exit 0
