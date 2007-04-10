
rem Debug script
rem Tool: 	gdb-insight
rem JTAG: 	USB_Olimex
rem 
rem Eduardo Romero
rem 13/02/2007



start c:\cygwin\usr\local\bin\openocd_USB_s5info  -farm7_olimex_usb.cfg

ping localhost

rem c:\cygwin\usr\local\GNUARM\bin\arm-elf-gdb.exe sample_app.elf
c:\cygwin\usr\local\GNUARM\bin\arm-elf-insight.exe sample_app.elf
