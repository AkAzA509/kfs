#include <drivers/console.h>
#include <kernel/log.h>
#include <fs/vfs.h>
#include <stdint.h>
#include <stdio.h>

kfile_t kfile_table[MAX_FD] = { 0 };

void add_fd_entry(u8_t idx, fd_type_t type, int flags)
{
	if (idx >= MAX_FD) {
		klog("Error: Bad fd canno't add %d", idx);
		return;
	}
	// if (kfile_table[idx].) {
	// 	klog("Error: fd table full, canno't add %d\n", idx);
	// 	return ;
	// }

	kfile_table[idx] = (kfile_t){
		.type = type, .flags = flags, .offset = 0, .priv_data = NULL
	};
}

void init_vfs(void)
{
	add_fd_entry(0, FD_TYPE_CONSOLE, O_RDONLY); // stdin
	add_fd_entry(1, FD_TYPE_CONSOLE, O_WRONLY); // stdout
	add_fd_entry(2, FD_TYPE_SERIAL, O_WRONLY); // stderr
	add_fd_entry(3, FD_TYPE_CONSOLE, O_WRONLY); // 1st vterm
	add_fd_entry(4, FD_TYPE_CONSOLE, O_WRONLY); // 2nd vterm
	add_fd_entry(5, FD_TYPE_CONSOLE, O_WRONLY); // 3rd vterm
	add_fd_entry(6, FD_TYPE_CONSOLE, O_WRONLY); // 4th vterm

	set_term_color(make_color(COLOR_LIGHT_RED, COLOR_BLACK));
	printf(BOOT_LOG "fd initialized\n");
	set_term_color(make_color(COLOR_WHITE, COLOR_BLACK));
}