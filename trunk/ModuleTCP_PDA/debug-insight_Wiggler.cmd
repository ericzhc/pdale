
rem Debug script
rem Tool: 	gdb-insight
rem JTAG: 	Wiggler
rem 
rem Eduardo Romero
rem 13/02/2007

start c:\cygwin\usr\local\bin\OcdLibRemote.exe -c Arm7 -d Wiggler &

c:\cygwin\usr\local\GNUARM\bin\arm-elf-insight sample_app.elf

