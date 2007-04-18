# Make command to use for dependencies
MAKECMD=make

# Configuration options:
#    Release
#    Debug
# If no configuration is specified, "Debug" will be used
ifndef "CFG"
CFG=Debug
endif


#
# variables used by the implicit make rules
#
PROG_PREFIX=arm-elf-

AR=$(PROG_PREFIX)ar
AS=$(PROG_PREFIX)as
CC=$(PROG_PREFIX)gcc
CPP=$(PROG_PREFIX)cpp
CXX=$(PROG_PREFIX)g++
LD=$(PROG_PREFIX)ld
OBJCOPY=$(PROG_PREFIX)objcopy
OBJDUMP=$(PROG_PREFIX)objdump
STRIP=$(PROG_PREFIX)strip


# ROOT Directory
ROOT=./../../../../..

uC-CPU_DIR=$(ROOT)/uC-CPU
uC-LIB_DIR=$(ROOT)/uC-LIB
uCOS-II_SRC_DIR=$(ROOT)/uCOS-II/Source
uC-TCP_DIR=$(ROOT)/uC-TCPIP
uC-TCP_SRC_DIR=$(ROOT)/uC-TCPIP/Source
uC-TCP_CFG_DIR=$(ROOT)/uC-TCPIP/Cfg
uC-TCP_PKT_DIR=$(ROOT)/uC-TCPIP/IF
uC-TCP_IF_DIR=$(ROOT)/uC-TCPIP/IF/Ether
uC-TCP_OS_DIR=$(ROOT)/uC-TCPIP/OS/uCOS-II

uC-DHCP_DIR=$(ROOT)/uC-DHCPc/Source
uC-TFTP_SRC_DIR=$(ROOT)/uC-TFTP/Source
uC-TFTP_OS_DIR=$(ROOT)/uC-TFTP/OS/uCOS-II
uC-HTTPs_SRC_DIR=$(ROOT)/uC-HTTPs/Source
uC-HTTPs_OS_DIR=$(ROOT)/uC-HTTPs/OS/uCOS-II
uC-DNSc_SRC_DIR=$(ROOT)/uC-DNSc/Source

uCOS-II_SRC_DIR=$(ROOT)/uCOS-II/Source

PRJ_FILES= audioApp.o audioDrv.o erG711.o debugELR2.o 
LIB_FILES=uCOSII.a
INCLUDES=-I.

#
# Target specific options & directories


##############################################################################################
## CerfPDA
##############################################################################################

BSP_DIR=./../BSP
HAL_DIR=./../HAL
APP_DIR=.

uCOS-II_DIR=$(ROOT)/uCOS-II/Ports/ARM/CerfPDA/GNU
uCOS-II_PORT_DIR=$(ROOT)/uCOS-II/Ports/ARM/CerfPDA/GNU
uC-FS_DIR=$(ROOT)/uC-FS
uC-OSII_DIR=./uC-Lib/CerfPDA/uCOS-II

UCOSII_INC=  -I$(uCOS-II_DIR) -I$(uCOS-II_SRC_DIR) \
 -I$(BSP_DIR) -I$(uC-CPU_DIR) -I$(HAL_DIR)/Lib/include 
 
# modif ELR 16/5
SYS_INC=-I$(BSP_DIR) -I$(HAL_DIR)/Lib/include
#CERFPDA_INC=-I$(SS_UCLIB)

CERFPDA_FILES=_start_sa.o bsp.o bsp_a.o syscalls.o 

ASFLAGS=
CFLAGS= -g -mlittle-endian

#
# Configuration: Release
#
ifeq "$(CFG)" "Release"
OUTDIR=./Release
OBJ_DIR=./Release
OUTFILE=ss.elf
INCLUDES+=$(UCOSII_INC) $(SYS_INC) $(UCOSII_INC) $(CERFPDA_INC)
ASFLAGS += $(INCLUDES)
CFLAGS += $(INCLUDES) 
CFG_LIB= uCOSII.a \
	    ./../HAL/Lib/Release/Project.a \
	     -lc -lstdc++ 

VPATH = $(APP_DIR):$(BSP_DIR):$(uCOS-II_DIR):$(uCOS-II_SRC_DIR)
        
vpath %.o $(OBJ_DIR)

all:$(PRJ_FILES) $(CERFPDA_FILES)
	$(CC) -nostartfiles -o $(OUTDIR)/$(OUTFILE) \
		-Xlinker -Map -Xlinker $(OUTDIR)/ss.map \
		-T $(BSP_DIR)/CerfPDA_lnk_ram.lds \
		$(OBJ_DIR)/_start_sa.o \
		$(OBJ_DIR)/bsp.o \
		$(OBJ_DIR)/bsp_a.o \
		$(OBJ_DIR)/syscalls.o \
		$(OBJ_DIR)/audioDrv.o \
		$(OBJ_DIR)/erG711.o \
		$(OBJ_DIR)/audioApp.o \
		$(CFG_LIB)
	$(OBJCOPY) --strip-all "$(OUTDIR)/$(OUTFILE)" "$(OUTDIR)/ssS.elf"
	$(OBJCOPY) -O binary "$(OUTDIR)/$(OUTFILE)" "$(OUTDIR)/ss.bin"

endif       # end CerfPDA Release

#
# Configuration: Debug
#
ifeq "$(CFG)" "Debug"
OUTDIR=./Debug
OBJ_DIR=./Debug
OUTFILE=ss.elf
INCLUDES+=$(UCOSII_INC) $(SYS_INC) $(UCOSII_INC) $(CERFPDA_INC)
ASFLAGS += $(INCLUDES)
CFLAGS += $(INCLUDES) 
CFG_LIB= uCOSII.a \
	    ./../HAL/Lib/Debug/Project.a \
	     -lc -lstdc++ 

VPATH = $(APP_DIR):$(BSP_DIR):$(uCOS-II_DIR):$(uCOS-II_SRC_DIR)
vpath %.o $(OBJ_DIR)

all:$(CERFPDA_FILES) $(PRJ_TST_FILES) $(PRJ_FILES) 
	$(CC) -nostartfiles -o $(OUTDIR)/$(OUTFILE) \
		-Xlinker -Map -Xlinker $(OUTDIR)/ss.map \
		-T $(BSP_DIR)/CerfPDA_lnk_ram.lds \
		$(OBJ_DIR)/_start_sa.o \
		$(OBJ_DIR)/bsp.o \
		$(OBJ_DIR)/bsp_a.o \
		$(OBJ_DIR)/syscalls.o \
		$(OBJ_DIR)/audioApp.o \
		$(OBJ_DIR)/audioDrv.o \
		$(OBJ_DIR)/erG711.o \
		$(OBJ_DIR)/debugELR2.o \
		$(CFG_LIB)
	$(OBJCOPY) --strip-all "$(OUTDIR)/$(OUTFILE)" "$(OUTDIR)/ssS.elf"
	$(OBJCOPY) -O binary "$(OUTDIR)/$(OUTFILE)" "$(OUTDIR)/ss.bin"

endif       # end CerfPDA Debug section

_start_sa.o: _start_sa.s start_sa.h
	$(AS) $(ASFLAGS) -o $(OBJ_DIR)/_start_sa.o _start_sa.s

bsp.o: bsp.c bsp.h ucos_ii.h app_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/bsp.o $(BSP_DIR)/bsp.c

bsp_a.o: bsp_a.s
	$(AS) $(ASFLAGS) -o $(OBJ_DIR)/bsp_a.o $(BSP_DIR)/bsp_a.s
	
syscalls.o: syscalls.c 
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/syscalls.o $(BSP_DIR)/syscalls.c

debugELR2.o: debugELR2.c debugELR2.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/debugELR2.o $(APP_DIR)/debugELR2.c

audioDrv.o: audioDrv.c audioDrv.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/audioDrv.o $(APP_DIR)/audioDrv.c

audioApp.o: audioApp.c audioDrv.h debugELR2.h app_cfg.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/audioApp.o audioApp.c
	
erG711.o: erG711.c erG711.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/erG711.o $(APP_DIR)/erG711.c

$(OUTDIR):
	mkdir -p "$(OUTDIR)"

$(OBJ_DIR):
	mkdir -p "$(OBJ_DIR)"


# Clean this project
clean:
	rm -f $(OUTDIR)/$(OUTFILE)
	rm -f $(OBJ_DIR)/*.o

# Clean this project and all dependencies
cleanall: clean

