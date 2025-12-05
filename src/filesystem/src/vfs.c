#include <vfs.h>
#include <filesystem.h>
#include <printf.h>
#include <string.h>

#define WARN(...) printf("[VFS]"__VA_ARGS__)

inode* parse_path(const path_t path);
int get_path_first_token(const path_t path);
inode* get_son_inode(inode* father, char * son_name);


inode* current_inodes_table[MAX_OPEN_FILES] = {0};
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

inode* get_son_inode(inode* father, char* son_name) {
    inode* current_son;
    current_son = father->first_son;
    while(0 != current_son) {
        if (strcmp(son_name, current_son->name) == 0) {
            return current_son;
        } else {
            current_son = current_son->brother;
        }
    }
    WARN("file does not exist\n");
    return 0;
}

inode* parse_path(const path_t path) {
    int token_size;
    path_t local_path;
    inode* current_inode;
    // unsigned current_pos = 0;

    if(path[0] != '/') {
        WARN("invalid path\n");
        return 0;
    }

    current_inode = &(RAM_inodes_table[ROOT_INODE]);
    local_path = &(path[1]);
    while(1) {
        token_size = get_path_first_token(local_path);
        if(token_size < 0) {
            return 0;
        }

        if(token_size > 0) {
            current_inode = get_son_inode(current_inode, token);
        }

        if(0 == current_inode) {
            WARN("file not found : %s", token);
            return 0;
        }

        if(FILE == current_inode->type) {
            if('\0' != local_path[token_size]) {
                WARN("Path contain a file\n");
                return 0;
            } else {
                return current_inode;
            }
        } else if (EMPTY == current_inode->type) {
            WARN("Empty node");
            return 0;
        }

        if('\0' == local_path[token_size]) {
            return current_inode;
        } else {
            local_path = &(local_path[token_size+1]);
        }
    }
}


vfs_error create_file(const path_t path, const char *file) {
    inode* dir;
    inode_id new_inode;

    if('\0' == file[0] || strlen(file) > MAX_SIZE_NAME) {
        WARN("Invalid file name\n");
        return VFS_INVALID_NAME;
    }
    
    dir = parse_path(path);
    if(0 == dir) {
        return VFS_INVALID_FD;
    }
    if(FOLDER != dir->type) {
        WARN("Parent folder is not a folder\n");
        return VFS_INVALID_FD;
    }

    new_inode = alloc_inode(file, dir, FILE);
    if(new_inode < 0) {
        return VFS_MEMORY_FULL;
    }

    return VFS_OK;
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

