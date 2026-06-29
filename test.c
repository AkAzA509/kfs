#include <stdio.h>

int main() {
	
	__uint16_t test =(0x04 << 8) | 0x63;
	
	printf("size: %#x\n", test);
	// printf("size: %x\n");
}