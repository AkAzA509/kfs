# kfs

# Compilation
Flags:

	-fno-builtin			: disables the optimization that replaces standard C library function calls with specialized inline code
	-fno-exceptions		 	: disables C++ exception handling support
	-fno-stack-protector	: explicitly disables Stack Smashing Protector (SSP) or stack canary protection
	-fno-rtti				: disables the generation of Run-Time Type Information (RTTI) for classes with virtual functions
	-nostdlib				: do not use standard system startup files or libraries when linking (libc ...)
	-nodefaultlibs			: do not use the standard system libraries when linking (libc ...)