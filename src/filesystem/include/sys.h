#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	 13

#ifndef __ASSEMBLER__
#include "vfs.h"

void sys_write(char * buf);
int sys_fork();

vfs_error sys_create_file(const path_t path, const char *file);

vfs_error sys_delete_file(const path_t path);

vfs_error sys_close_file(const fd_t *fd);

long sys_read_file(const fd_t *fd, char *str, unsigned len);

long sys_write_file(const fd_t *fd, char *str, unsigned len);

long sys_append_file(fd_t *fd, char *str, unsigned len);

vfs_error sys_create_dir(path_t path, char *dir);

vfs_error sys_delete_dir(path_t dir);

vfs_error sys_read_dir(path_t dir, char **files[FILE_NAME_SIZE], unsigned *nb_entries);

#endif

#endif  /*_SYS_H */
