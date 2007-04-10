
#********************************************************************************************************
#                                                uC/OS-II
#                                          The Real-Time Kernel
#
#                               (c) Copyright 2004, Micrium, Inc., Weston, FL
#                                           All Rights Reserved
#
#                                           Debug Startup Script
#
#
#
# File         : .gdbinit
# Originally by: Jean-Denis Hatier
#
#********************************************************************************************************

echo Setting up Micrium debugging environment...\n

dir .
set prompt (arm-ocd-gdb)

# Set max number of complaints about incorrect symbols.
set complaints 1                                

# Set default output radix for printing of values.
set output-radix 16                             

# Set default input radix for entering numbers.
set input-radix 16                              

# This connects to OCDLibRemote default port (8888).
target remote localhost:8888                    

# Increase the packet size to improve download speed.
set remote memory-write-packet-size 1024        
set remote memory-write-packet-size fixed

#c

# Load the program executable.
load

# Set a breakpoint at main().
b main

# Execute until main().
c                                               
