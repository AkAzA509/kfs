

# What is a segment

Segmented Addressing
The processor uses byte addressing. This means memory is organized and accessed as a sequence of bytes.
Whether one or more bytes are being accessed, a byte address is used to locate the byte or bytes memory. The
range of memory that can be addressed is called an address space.
The processor also supports segmented addressing. This is a form of addressing where a program may have many
independent address spaces, called segments. For example, a program can keep its code (instructions) and stack
in separate segments. Code addresses would always refer to the code space, and stack addresses would always
refer to the stack space. The following notation is used to specify a byte address within a segment:
Segment-register:Byte-address
For example, the following segment address identifies the byte at address FF79H in the segment pointed by the DS
register:
DS:FF79H
The following segment address identifies an instruction address in the code segment. The CS register points to the
code segment and the EIP register contains the address of the instruction.
CS:EIP