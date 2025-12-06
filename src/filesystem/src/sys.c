#include "fork.h"
#include "printf.h"
#include "utils.h"
#include "sched.h"
#include "mm.h"
#include "sys.h"
#include "vfs.h"

void sys_write(char * buf){
	printf(buf);
}

int sys_fork(){
	return copy_process(0, 0, 0);
}

void sys_exit(){
	exit_process();
}

vfs_error sys_create_file(const path_t path, const char *file) {
	return create_file(path, file);	
}

vfs_error sys_delete_file(const path_t path) {
	return delete_file(path);	
}

vfs_error sys_open_file(const path_t path, fd_t *fd) {
	return open_file(path, fd);
}

vfs_error sys_close_file(fd_t *fd) {
	return close_file(fd);
}

long sys_read_file(const fd_t *fd, char *str, unsigned len) {
	return read_file(fd, str, len);
}

long sys_write_file(const fd_t *fd, char *str, unsigned len) {
	return write_file(fd, str, len);
}

long sys_append_file(fd_t *fd, char *str, unsigned len) {
	return append_file(fd, str, len);
}

vfs_error sys_create_dir(path_t path, char *dir) {
	return create_dir(path, dir);
}

vfs_error sys_delete_dir(path_t dir) {
	return delete_dir(dir);
}

vfs_error sys_read_dir(path_t dir, char **files[FILE_NAME_SIZE], unsigned *nb_entries) {
	return read_dir(dir, files, nb_entries);
}

void * const sys_call_table[__NR_syscalls] = {
	sys_write, 
	sys_fork, 
	sys_exit,
	
	sys_create_file,
	sys_delete_file,
	sys_open_file,
	sys_close_file,
	sys_read_file,
	sys_write_file,
	sys_append_file,

	sys_create_dir,
	sys_delete_dir,
	sys_read_dir
};
