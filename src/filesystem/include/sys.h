#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	 15

#ifndef __ASSEMBLER__
#include "vfs.h"

void sys_write(char * buf);
int sys_fork();

vfs_error sys_create_file(const path_t path, const char *file);

vfs_error sys_delete_file(const path_t path);

vfs_error sys_close_file(fd_t *fd);

long sys_read_file(const fd_t *fd, char *str, unsigned len);

long sys_write_file(const fd_t *fd, char *str, unsigned len);

long sys_append_file(fd_t *fd, char *str, unsigned len);

vfs_error sys_create_dir(path_t path, char *dir);

vfs_error sys_delete_dir(path_t dir);

vfs_error sys_open_dir(const path_t path, fd_t *fd);

vfs_error sys_close_dir(fd_t *fd);

vfs_error sys_get_dir_son(fd_t *fd, char *son_name);

#endif

#endif  /*_SYS_H */
