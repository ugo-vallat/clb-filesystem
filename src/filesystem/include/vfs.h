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
    VFS_INVALID_PATH,
    VFS_INVALID_FD,
    VFS_INVALID_PATH_DIR,
    VFS_INVALID_PATH_FILE,
    VFS_MAX_FD_REACHED,
    VFS_MEMORY_FULL,
    VFS_UNKWON_ERROR,

} vfs_error;

typedef struct filedescriptor_s fd_t;
typedef char* path_t;


/**
 * @brief Create a file object
 * 
 * @param file 
 * @return vfs_error 
 */
vfs_error create_file(path_t file);

/**
 * @brief Delete a file object
 * 
 * @param fd 
 * @return vfs_error 
 */
vfs_error delete_file(fd_t *fd);

/**
 * @brief Open a file object
 * 
 * @param path 
 * @param fd 
 * @return vfs_error 
 */
vfs_error open_file(path_t path, fd_t *fd);

/**
 * @brief Close a file object
 * 
 * @param fd 
 * @return vfs_error 
 */
vfs_error close_file(fd_t *fd);

/**
 * @brief Read a file object
 * 
 * @param fd 
 * @return vfs_error 
 */
vfs_error read_file(fd_t *fd);

/**
 * @brief write a file object (overwrite)
 * 
 * @param fd 
 * @return vfs_error 
 */
vfs_error write_file(fd_t *fd);

/**
 * @brief write at the end of a file object (not overwrite)
 * 
 * @param fd 
 * @return vfs_error 
 */
vfs_error append_file(fd_t *fd);



/**
 * @brief Create a dir object
 * 
 * @param dir 
 * @return vfs_error 
 */
vfs_error create_dir(path_t dir);

/**
 * @brief delete a dir object
 * 
 * @param dir 
 * @return vfs_error 
 */
vfs_error delete_dir(path_t dir);

/**
 * @brief read a dire object, to list current files in the dir
 * 
 * @param fd 
 * @return vfs_error 
 */
vfs_error read_dir(path_t dir);







#endif // _VFS_H_