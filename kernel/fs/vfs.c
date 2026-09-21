#include <drivers/console.h>
#include <core/log.h>
#include <fs/vfs.h>

#include <stdint.h>

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
		.type = type,
		.flags = flags,
		.offset = 0,
		.priv_data = NULL,
	};
}

void init_vfs(void)
{
	add_fd_entry(0, FD_TYPE_CONSOLE, O_RDONLY); // stdin
	add_fd_entry(1, FD_TYPE_CONSOLE, O_WRONLY); // stdout
	add_fd_entry(2, FD_TYPE_SERIAL, O_WRONLY); // stderr

	for (u8_t i = 0; i < MAX_SCREENS; ++i) {
		add_fd_entry(i, FD_TYPE_CONSOLE, O_WRONLY);
	}
}
