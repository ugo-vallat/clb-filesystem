#ifndef _VFS_H_
#define _VFS_H_

/* 
    -----------------------------
    Virtual File System interface
    -----------------------------
*/

#define PATH_MAX_SIZE   

typedef enum {
    VFS_OK,
    VFS_ERROR,

} vfs_error;

typedef struct filedescriptor_s fd_t;
typedef char* path_t;

vfs_error create_file(path_t file);
vfs_error delete_file(fd_t *fd);
vfs_error open_file(path_t path, fd_t *fd);
vfs_error close_file(fd_t *fd);
vfs_error read_file(fd_t *fd);
vfs_error write_file(fd_t *fd);
vfs_error append_file(fd_t *fd);

vfs_error create_dir(path_t dir);
vfs_error delete_dir(path_t dir);
vfs_error read_dir(fd_t *fd);







#endif // _VFS_H_