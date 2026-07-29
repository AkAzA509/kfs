#include "arch/i386/console.h"
#include <kernel/common.h>
#include <kernel/shell.h>
#include <kernel/log.h>
#include <kernel/io.h>
#include <string.h>
#include <stdio.h>

#define CURRENT_YEAR 2026

enum {
	cmos_address = 0x70,
	cmos_data = 0x71
};

int get_update_in_progress_flag() {
	outb(cmos_address, 0x0A);
	return (inb(cmos_data) & 0x80);
}

unsigned char get_RTC_register(int reg) {
	outb(cmos_address, reg);
	return inb(cmos_data);
}

void read_rtc() {
	int century_register = 0x00;
	unsigned char	century;
	unsigned char	last_second, second;
	unsigned char	last_minute, minute;
	unsigned char	last_hour, hour;
	unsigned char	last_day, day;
	unsigned char	last_month, month;
	unsigned char	last_year;
	unsigned int	year;
	unsigned char	last_century;
	unsigned char	registerB;

	// Note: This uses the "read registers until you get the same values twice in a row" technique
	// to avoid getting dodgy/inconsistent values due to RTC updates
	while (get_update_in_progress_flag()); // Make sure an update isn't in progress
	second = get_RTC_register(0x00);
	minute = get_RTC_register(0x02);
	hour = get_RTC_register(0x04);
	day = get_RTC_register(0x07);
	month = get_RTC_register(0x08);
	year = get_RTC_register(0x09);
	if(century_register != 0)
		century = get_RTC_register(century_register);

	do {
		last_second = second;
		last_minute = minute;
		last_hour = hour;
		last_day = day;
		last_month = month;
		last_year = year;
		last_century = century;

		while (get_update_in_progress_flag()); // Make sure an update isn't in progress
		second = get_RTC_register(0x00);
		minute = get_RTC_register(0x02);
		hour = get_RTC_register(0x04);
		day = get_RTC_register(0x07);
		month = get_RTC_register(0x08);
		year = get_RTC_register(0x09);
		if(century_register != 0)
			century = get_RTC_register(century_register);
	} while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
		(last_day != day) || (last_month != month) || (last_year != year) ||
		(last_century != century) );

	registerB = get_RTC_register(0x0B);

	// Convert BCD to binary values if necessary
	if (!(registerB & 0x04)) {
		second = (second & 0x0F) + ((second / 16) * 10);
		minute = (minute & 0x0F) + ((minute / 16) * 10);
		hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
		day = (day & 0x0F) + ((day / 16) * 10);
		month = (month & 0x0F) + ((month / 16) * 10);
		year = (year & 0x0F) + ((year / 16) * 10);
		if(century_register != 0)
			century = (century & 0x0F) + ((century / 16) * 10);
	}

	// Convert 12 hour clock to 24 hour clock if necessary
	if (!(registerB & 0x02) && (hour & 0x80))
		hour = ((hour & 0x7F) + 12) % 24;

	// Calculate the full (4-digit) year
	if(century_register != 0)
		year += century * 100;
	else {
		year += (CURRENT_YEAR / 100) * 100;
		if(year < CURRENT_YEAR)
			year += 100;
	}
	printf("date: %.2u/%.2u/%u time: %.2uh%.2u\n", day, month, year, (hour + 2) % 24, minute);
}

static void	cmd_reboot(void) { REBOOT }
static void	cmd_halt(void) { __asm__ volatile ("cli"); HALT_ERROR; }
static void	cmd_print_logo(void) { ASCII_LOGO; }
static void	cmd_print_stack(void){ log_stack(0); }
static void	cmd_shutdown(void) { SHUTDOWN; }
static void	cmd_date(void) { read_rtc(); }
static void	cmd_clear(void) { screen_clear(); }
static void	cmd_help(void) {
	printf("here is a list of the command available on this shell\n\n");
	printf("reboot             reboot the os\n");
	printf("shutdown           shutdown the os\n");
	printf("halt               halt the cpu and lock it (for now)\n");
	printf("plogo              print the kernel boot logo\n");
	printf("pstack <val>       print the kernel stack, by default all the stack is displayed\n");
	printf("                   add a positive val to chose the number of element displayed\n");
	printf("date               display the current date in UTC+2 format\n");
	printf("help               display this informations\n");
	printf("clear              not yet available\n");
}

static const char	*cmd_table[] = {
	"reboot", "halt", "plogo", "pstack", "shutdown", "date", "help", "clear", NULL
};

typedef void (*cmd_handler_t)(void);
static const cmd_handler_t	cmd_handlers[] = {
	cmd_reboot, cmd_halt, cmd_print_logo, cmd_print_stack, cmd_shutdown, cmd_date, cmd_help , cmd_clear
};

void	shell_execute(const char *input, size_t len)
{
	for (u16_t i = 0; cmd_table[i]; i++) {
		if (strncmp(input, cmd_table[i], len) == 0 && cmd_table[i][len] == '\0') {
			cmd_handlers[i]();
			return ;
		}
	}
	printf("commande not found: %.*s\n", (int)len, input);
}

void	print_prompt(void)
{
	printf("Tekos/root > ");
}