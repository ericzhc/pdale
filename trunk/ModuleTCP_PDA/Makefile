# Philippe Mabilleau ing.
# 11/03/2005

BOOTROOT=$(shell pwd)

ROOT=./../../../../../

uC-CPU_DIR=$(ROOT)/uC-CPU
uC-CPU_ARCH_DIR=$(ROOT)/uC-CPU/ARM7/ARM/GNU
uC-LIB_DIR=$(ROOT)/uC-LIB
uCOS-II_DIR=$(ROOT)/uCOS-II/Ports/ARM/Generic/GNU
uCOS-II_SRC_DIR=$(ROOT)/uCOS-II/Source
uC-TCP_DIR=$(ROOT)/uC-TCPIP
uC-TCP_SRC_DIR=$(ROOT)/uC-TCPIP/Source
uC-TCP_CFG_DIR=$(ROOT)/uC-TCPIP/Cfg
uC-TCP_PKT_DIR=$(ROOT)/uC-TCPIP/IF
uC-TCP_IF_DIR=$(ROOT)/uC-TCPIP/IF/Ether
uC-TCP_NIC_DIR=$(ROOT)/uC-TCPIP/NIC/Ether/LAN91C111
uC-TCP_OS_DIR=$(ROOT)/uC-TCPIP/OS/uCOS-II
BSP_DIR=./../BSP
LH79520_DIR=$(ROOT)/CPU/Sharp/LH79520
uC-DHCP_DIR=$(ROOT)/uC-DHCPc/Source
uC-FS_DIR=./../uC-FS/FS/API
uC-TFTP_SRC_DIR=$(ROOT)/uC-TFTP/Source
uC-TFTP_OS_DIR=$(ROOT)/uC-TFTP/OS/uCOS-II
uC-HTTPs_SRC_DIR=$(ROOT)/uC-HTTPs/Source
uC-HTTPs_OS_DIR=$(ROOT)/uC-HTTPs/OS/uCOS-II

uC-FS_LIB=./../uC-FS/LIB/ucfs.a

OBJ_DIR=./obj

VPATH = $(uC-CPU_ARCH_DIR):$(uC-LIB_DIR):$(uCOS-II_DIR):$(uCOS-II_SRC_DIR):$(uC-TCP_SRC_DIR):$(LH79520_DIR): \
        $(uC-TCP_IF_DIR):$(uC-TCP_NIC_DIR):$(uC-TCP_OS_DIR):$(BSP_DIR):$(uC-CPU_DIR):$(uC-TCP_PKT_DIR): \
	$(uC-DHCP_DIR):$(uC-FS_DIR):$(uC-TFTP_SRC_DIR):$(uC-TFTP_OS_DIR):$(uC-HTTPs_SRC_DIR):$(uC-HTTPs_OS_DIR)
vpath %.o $(OBJ_DIR)


PROG_PREFIX=arm-elf-
ASFLAGS += 
CFLAGS += -g -mcpu=arm7tdmi -mlittle-endian

#
# variables used by the implicit make rules
#
AR=$(PROG_PREFIX)ar
AS=$(PROG_PREFIX)as
CC=$(PROG_PREFIX)gcc
CPP=$(PROG_PREFIX)cpp
CXX=$(PROG_PREFIX)g++
LD=$(PROG_PREFIX)ld
OBJCOPY=$(PROG_PREFIX)objcopy
OBJDUMP=$(PROG_PREFIX)objdump
STRIP=$(PROG_PREFIX)strip

INCLUDES += -I. -I$(uC-CPU_ARCH_DIR) -I$(uC-LIB_DIR) -I$(uCOS-II_DIR) -I$(uCOS-II_SRC_DIR) -I$(uC-TCP_SRC_DIR) \
 -I$(uC-TCP_IF_DIR) -I$(uC-TCP_NIC_DIR) -I$(uC-TCP_OS_DIR) -I$(BSP_DIR) -I$(uC-CPU_DIR) -I$(uC-TCP_PKT_DIR) \
 -I$(LH79520_DIR) -I$(uC-DHCP_DIR) -I$(uC-FS_DIR) -I$(uC-TFTP_SRC_DIR) -I$(uC-TFTP_OS_DIR) \
 -I$(uC-HTTPs_SRC_DIR) -I$(uC-HTTPs_OS_DIR)

ASFLAGS += $(INCLUDES)
CFLAGS += $(INCLUDES) 

all: net_udp.o net_tmr.o net_nic.o os_core.o os_mem.o net_tcp.o os_mutex.o bsp.o net_if.o os_cpu_a.o \
 lib_str.o net_util.o os_dbg.o net_stat.o net_sock.o net_buf.o net.o os_flag.o net_ip.o net_icmp.o \
 os_task.o sockettcp.o app.o net_os.o lib_mem.o net_bsp.o net_ctr.o net_conn.o net_if_pkt.o os_cpu_c.o cpu_a.o \
 net_ascii.o os_sem.o net_arp.o os_q.o os_time.o bsp_a.o net_dbg.o serial.o syscalls.o os_tmr.o \
 net_bsd.o dhcp-c.o net_isr.o tftps.o tftps_os.o https.o https_os.o

	$(CC) -nostartfiles -o sample_app.elf \
		-T $(BSP_DIR)/Zoom_lnk_ram.lds \
		$(OBJ_DIR)/app.o \
		$(OBJ_DIR)/sockettcp.o \
		$(OBJ_DIR)/bsp_a.o \
		$(OBJ_DIR)/serial.o \
		$(OBJ_DIR)/syscalls.o \
		$(OBJ_DIR)/net_udp.o \
		$(OBJ_DIR)/net_tmr.o \
		$(OBJ_DIR)/net_nic.o \
		$(OBJ_DIR)/os_core.o \
		$(OBJ_DIR)/os_mem.o \
		$(OBJ_DIR)/net_tcp.o \
		$(OBJ_DIR)/os_mutex.o \
		$(OBJ_DIR)/bsp.o \
		$(OBJ_DIR)/net_if.o \
		$(OBJ_DIR)/os_cpu_a.o \
		$(OBJ_DIR)/lib_str.o \
		$(OBJ_DIR)/net_util.o \
		$(OBJ_DIR)/os_dbg.o \
		$(OBJ_DIR)/net_dbg.o \
		$(OBJ_DIR)/net_stat.o \
		$(OBJ_DIR)/net_sock.o \
		$(OBJ_DIR)/net_buf.o \
		$(OBJ_DIR)/net.o \
		$(OBJ_DIR)/os_flag.o \
		$(OBJ_DIR)/net_ip.o \
		$(OBJ_DIR)/net_icmp.o \
		$(OBJ_DIR)/os_task.o \
		$(OBJ_DIR)/net_os.o \
		$(OBJ_DIR)/lib_mem.o \
		$(OBJ_DIR)/net_bsp.o \
		$(OBJ_DIR)/net_ctr.o \
		$(OBJ_DIR)/net_conn.o \
		$(OBJ_DIR)/net_if_pkt.o \
		$(OBJ_DIR)/os_cpu_c.o \
		$(OBJ_DIR)/cpu_a.o \
		$(OBJ_DIR)/net_ascii.o \
		$(OBJ_DIR)/os_sem.o \
		$(OBJ_DIR)/os_time.o \
		$(OBJ_DIR)/net_arp.o \
		$(OBJ_DIR)/os_q.o \
		$(OBJ_DIR)/os_tmr.o \
		$(OBJ_DIR)/net_bsd.o \
		$(OBJ_DIR)/dhcp-c.o \
		$(OBJ_DIR)/net_isr.o \
		$(OBJ_DIR)/tftps.o \
		$(OBJ_DIR)/tftps_os.o \
		$(OBJ_DIR)/https.o \
		$(OBJ_DIR)/https_os.o \
		$(uC-FS_LIB) -lgcc
	$(OBJCOPY) --strip-all sample_app.elf sample.elf

net_udp.o : net_udp.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_udp.o \
		$(uC-TCP_SRC_DIR)/net_udp.c

net_tmr.o : net_tmr.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_tmr.o \
		$(uC-TCP_SRC_DIR)/net_tmr.c

net_nic.o : net_nic.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_nic.o \
		$(uC-TCP_NIC_DIR)/net_nic.c

os_core.o : os_core.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_core.o \
		$(uCOS-II_SRC_DIR)/os_core.c

os_tmr.o : os_tmr.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_tmr.o \
		$(uCOS-II_SRC_DIR)/os_tmr.c

os_mem.o : os_mem.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_mem.o \
		$(uCOS-II_SRC_DIR)/os_mem.c

net_tcp.o : net_tcp.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_tcp.o \
		$(uC-TCP_SRC_DIR)/net_tcp.c

os_mutex.o : os_mutex.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_mutex.o \
		$(uCOS-II_SRC_DIR)/os_mutex.c

bsp.o : bsp.c bsp.h ucos_ii.h LH79520_map.h SMA_types.h ARM_arch.h \
 LH79520_clcdc.h LH79520_dmac.h LH79520_gpio.h LH79520_iocon.h LH79520_lcdicp.h LH79520_mmu.h LH79520_pwm.h \
 LH79520_rcpc.h LH79520_rtc.h LH79520_sdrc.h LH79520_smc.h LH79520_ssp.h LH79520_timer.h LH79520_uart.h \
 LH79520_vic.h LH79520_wdt.h net.h app_cfg.h app.h sockettcp.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/bsp.o \
		$(BSP_DIR)/bsp.c

net_if.o : net_if.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_if.o \
		$(uC-TCP_IF_DIR)/net_if.c

os_mbox.o : os_mbox.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_mbox.o \
		$(uCOS-II_SRC_DIR)/os_mbox.c

os_cpu_a.o: os_cpu_a.s
	$(CC) $(ASFLAGS) -c -o $(OBJ_DIR)/os_cpu_a.o \
		$(uCOS-II_DIR)/os_cpu_a.s

lib_str.o : lib_str.c cpu.h cpu_def.h lib_def.h lib_str.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/lib_str.o \
		$(uC-LIB_DIR)/lib_str.c

net_util.o : net_util.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_util.o \
		$(uC-TCP_SRC_DIR)/net_util.c

os_dbg.o : os_dbg.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_dbg.o \
		$(uCOS-II_DIR)/os_dbg.c

net_stat.o : net_stat.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_stat.o \
		$(uC-TCP_SRC_DIR)/net_stat.c

net_sock.o : net_sock.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_sock.o \
		$(uC-TCP_SRC_DIR)/net_sock.c

net_buf.o : net_buf.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_buf.o \
		$(uC-TCP_SRC_DIR)/net_buf.c

net.o : net.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net.o \
		$(uC-TCP_SRC_DIR)/net.c

os_flag.o : os_flag.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_flag.o \
		$(uCOS-II_SRC_DIR)/os_flag.c

net_ip.o : net_ip.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_ip.o \
		$(uC-TCP_SRC_DIR)/net_ip.c

net_icmp.o : net_icmp.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_icmp.o \
		$(uC-TCP_SRC_DIR)/net_icmp.c

os_task.o : os_task.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_task.o \
		$(uCOS-II_SRC_DIR)/os_task.c

sockettcp.o : sockettcp.c app_cfg.h includes.h ucos_ii.h os_cpu.h os_cfg.h bsp.h LH79520_map.h SMA_types.h ARM_arch.h \
 LH79520_clcdc.h LH79520_dmac.h LH79520_gpio.h LH79520_iocon.h LH79520_lcdicp.h LH79520_mmu.h LH79520_pwm.h \
 LH79520_rcpc.h LH79520_rtc.h LH79520_sdrc.h LH79520_smc.h LH79520_ssp.h LH79520_timer.h LH79520_uart.h \
 LH79520_vic.h LH79520_wdt.h \
 net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h net_type.h net_dbg.h \
 net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h net_if.h net_if_pkt.h net_arp.h \
 net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/sockettcp.o sockettcp.c

app.o : app.c app_cfg.h includes.h ucos_ii.h os_cpu.h os_cfg.h bsp.h LH79520_map.h SMA_types.h ARM_arch.h \
 LH79520_clcdc.h LH79520_dmac.h LH79520_gpio.h LH79520_iocon.h LH79520_lcdicp.h LH79520_mmu.h LH79520_pwm.h \
 LH79520_rcpc.h LH79520_rtc.h LH79520_sdrc.h LH79520_smc.h LH79520_ssp.h LH79520_timer.h LH79520_uart.h \
 LH79520_vic.h LH79520_wdt.h \
 net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h net_type.h net_dbg.h \
 net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h net_if.h net_if_pkt.h net_arp.h \
 net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/app.o app.c

net_os.o : net_os.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_os.o \
		$(uC-TCP_OS_DIR)/net_os.c

lib_mem.o : lib_mem.c cpu.h cpu_def.h lib_def.h lib_str.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/lib_mem.o \
		$(uC-LIB_DIR)/lib_mem.c

net_bsp.o: net_bsp.c bsp.h ucos_ii.h LH79520_map.h SMA_types.h ARM_arch.h \
 LH79520_clcdc.h LH79520_dmac.h LH79520_gpio.h LH79520_iocon.h LH79520_lcdicp.h LH79520_mmu.h LH79520_pwm.h \
 LH79520_rcpc.h LH79520_rtc.h LH79520_sdrc.h LH79520_smc.h LH79520_ssp.h LH79520_timer.h LH79520_uart.h \
 LH79520_vic.h LH79520_wdt.h net.h app_cfg.h app.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_bsp.o \
		$(BSP_DIR)/net_bsp.c

net_isr.o: net_isr.c bsp.h ucos_ii.h LH79520_map.h SMA_types.h ARM_arch.h \
 LH79520_clcdc.h LH79520_dmac.h LH79520_gpio.h LH79520_iocon.h LH79520_lcdicp.h LH79520_mmu.h LH79520_pwm.h \
 LH79520_rcpc.h LH79520_rtc.h LH79520_sdrc.h LH79520_smc.h LH79520_ssp.h LH79520_timer.h LH79520_uart.h \
 LH79520_vic.h LH79520_wdt.h net.h app_cfg.h app.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_isr.o \
		$(BSP_DIR)/net_isr.c

net_ctr.o : net_ctr.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_ctr.o \
		$(uC-TCP_SRC_DIR)/net_ctr.c

net_conn.o : net_conn.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_conn.o \
		$(uC-TCP_SRC_DIR)/net_conn.c

net_if_pkt.o : net_if_pkt.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_if_pkt.o \
		$(uC-TCP_PKT_DIR)/net_if_pkt.c

os_cpu_c.o : os_cpu_c.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_cpu_c.o \
		$(uCOS-II_DIR)/os_cpu_c.c

cpu_a.o: cpu_a.s
	$(CC) $(ASFLAGS) -c -o $(OBJ_DIR)/cpu_a.o \
		$(uC-CPU_ARCH_DIR)/cpu_a.s

net_ascii.o : net_ascii.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_ascii.o \
		$(uC-TCP_SRC_DIR)/net_ascii.c

os_sem.o : os_sem.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_sem.o \
		$(uCOS-II_SRC_DIR)/os_sem.c

os_time.o : os_time.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_time.o \
		$(uCOS-II_SRC_DIR)/os_time.c

net_arp.o : net_arp.c net.h cpu.h cpu_def.h lib_def.h lib_mem.h net_def.h net_nic_def.h net_cfg.h net_cfg_net.h \
 net_type.h net_dbg.h net_err.h net_ctr.h net_util.h net_stat.h net_tmr.h net_buf.h net_os.h ucos_ii.h \
 os_cpu.h os_cfg.h net_if.h net_if_pkt.h net_arp.h net_nic.h net_ip.h net_icmp.h net_udp.h net_tcp.h \
 net_ascii.h net_bsd.h net_sock.h net_conn.h net_dbg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_arp.o \
		$(uC-TCP_SRC_DIR)/net_arp.c

os_q.o : os_q.c ucos_ii.h  os_cpu.h os_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/os_q.o \
		$(uCOS-II_SRC_DIR)/os_q.c

bsp_a.o: bsp_a.s
	$(CC) $(ASFLAGS) -c -o $(OBJ_DIR)/bsp_a.o \
		$(BSP_DIR)/bsp_a.s

net_dbg.o : net_dbg.c net_dbg.h net.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_dbg.o \
		$(uC-TCP_SRC_DIR)/net_dbg.c

serial.o: serial.c serial.h
	$(CC) $(CCFLAGS) -c -o $(OBJ_DIR)/serial.o \
		$(BSP_DIR)/serial.c

syscalls.o: syscalls.c serial.h
	$(CC) $(CCFLAGS) -c -o $(OBJ_DIR)/syscalls.o \
		$(BSP_DIR)/syscalls.c

net_bsd.o : net_bsd.c net_bsd.h 
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/net_bsd.o \
		$(uC-TCP_SRC_DIR)/net_bsd.c

dhcp-c.o : dhcp-c.c dhcp-c.h 
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/dhcp-c.o \
		$(uC-DHCP_DIR)/dhcp-c.c

tftps.o : tftps.c tftp.h 
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/tftps.o \
		$(uC-TFTP_SRC_DIR)/tftps.c

tftps_os.o : tftps_os.c tftps_os_cfg.h 
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/tftps_os.o \
		$(uC-TFTP_OS_DIR)/tftps_os.c

https.o : http-s.c http-s.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/https.o \
		$(uC-HTTPs_SRC_DIR)/http-s.c

https_os.o : http-s_os.c ../../Cfg/Template/http-s_cfg.h
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/https_os.o \
		$(uC-HTTPs_OS_DIR)/http-s_os.c

clean:
	-rm -f $(OBJ_DIR)/*.o
