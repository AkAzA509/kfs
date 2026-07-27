#ifndef COMMON_H
#define COMMON_H

#include <kernel/io.h>

 // #define ASCII_LOGO 	printf(" _____    _     ___      \n");
// 	printf("|_   _|__| | __/ _ \\ ___ \n");
// 	printf("  | |/ _ \\ |/ / | | / __|\n");
// 	printf("  | |  __/   <| |_| \\__ \\\n");
// 	printf("  |_|\\___|_|\\_\\\\___/|___/\n");
// 	printf("_________________________\n");


#define ASCII_LOGO printf("  _____    _     ___      \n \
|_   _|__| | __/ _ \\ ___ \n \
  | |/ _ \\ |/ / | | / __|\n \
  | |  __/   <| |_| \\__ \\\n \
  |_|\\___|_|\\_\\\\___/|___/\n \
_________________________\n"); \

#define HALT_ERROR do { \
	__asm__ volatile("hlt"); \
} while (1) \

#define SHUTDOWN outw(0x604, 0x2000)

#define REBOOT u8_t good = 0x02; \
	while (good & 0x02)   \
		good = inb(0x64); \
	outb(0x64, 0xFE);     \
	__asm__ volatile ("hlt"); \

#endif // COMMON_H