#include <vfs.h>
#include <filesystem.h>
#include <printf.h>

#define WARN(...) printf("[VFS]"__VA_ARGS__)

inode parse_path(const path_t path);
int get_path_first_token(const path_t path);


inode *current_inodes_table[MAX_OPEN_FILES] = {0};
static char token[MAX_SIZE_NAME+1];



int get_path_first_token(const path_t path) {
    
    int i;
    for(i = 0; i <= MAX_SIZE_NAME; i++) {
        switch (path[i]) {
            case '\0':
            case '/':
                token[i] = '\0';
                return i;
            default:
                token[i] = path[i];
                break;
        }
    }

    WARN("name too long\n");
    return -1;
}

inode parse_path(const path_t path) {
    int token_size;
    path_t local_path;
    inode *current_inode;
    // unsigned current_pos = 0;

    if(path[0] != '/') {
        WARN("invalid path\n");
        return 0;
    }

    current_inode = ROOT_INODE;
    local_path = &(path[1]);
    token_size = get_path_first_token(local_path);
}


vfs_error create_file(const path_t path, const char *file) {

}

vfs_error delete_file(const path_t path);

vfs_error open_file(const path_t path, fd_t *fd);

vfs_error close_file(const fd_t *fd);

long read_file(const fd_t *fd, char *str, unsigned len);

long write_file(const fd_t *fd, const char *str, unsigned len);

long append_file(fd_t *fd, const char *str, unsigned len);


/*
    -------------------
    Directory managment
    -------------------
*/

vfs_error create_dir(path_t path, char *dir);

vfs_error delete_dir(path_t dir);

vfs_error read_dir(path_t dir, char **files[FILE_NAME_SIZE], unsigned *nb_entries);

