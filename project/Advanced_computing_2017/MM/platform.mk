CC:=~/Advanced_computing_2017/gnueabi/opt/ext-toolchain/bin/arm-linux-gnueabihf-gcc

# Test to see if the platform is Windows (it's Windows if the shell has the .exe extention).
ifeq (.exe, $(suffix $(SHELL)))
	RM:=del /f
	CONCATENATE:=&
	MKDIR:=mkdir
	CP:=copy
else
	RM:=rm -f
	CONCATENATE:=;
	MKDIR:=mkdir -p
	CP:=cp
endif	
