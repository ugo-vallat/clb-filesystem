#ifndef	_USER_SYS_H
#define	_USER_SYS_H

#include "vfs.h"

void call_sys_write(char * buf);
int call_sys_fork();
void call_sys_exit();

vfs_error call_create_file(const path_t path, const char *file);
vfs_error call_delete_file(const path_t path);
vfs_error call_open_file(const path_t path, fd_t *fd);
vfs_error call_close_file(fd_t *fd);
long call_read_file(const fd_t *fd, char *str, unsigned len);
long call_write_file(const fd_t *fd, const char *str, unsigned len);
long call_append_file(fd_t *fd, const char *str, unsigned len);

vfs_error call_create_dir(path_t path, char *dir_name);
vfs_error call_delete_dir(path_t path);
vfs_error call_open_dir(const path_t path, fd_t *fd);
vfs_error call_close_dir(fd_t *fd);
vfs_error call_get_dir_son(fd_t *fd, char *son_name);


extern void user_delay ( unsigned long);
extern unsigned long get_sp ( void );
extern unsigned long get_pc ( void );

#endif  /*_USER_SYS_H */
