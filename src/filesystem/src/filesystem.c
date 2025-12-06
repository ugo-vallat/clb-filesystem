

#define __FILESYSTEM_C__
#include "../include/filesystem.h" // Don't move this fucking include if u want to stay alive :)
#include "../include/mm.h"



int memwrite(unsigned long src, unsigned long dst, unsigned long n);

inode_id find_last_child(inode *father) {
  if (father == 0)
    return -1;

  if (!father->first_son)
    return -1;

  inode *cur = father->first_son;
  while (cur->brother)
    cur = cur->brother;

  return cur->id;
}

void copy_str(char *dest, char *src, int size) {
  for (int i = 0; i < size; i++) {
    dest[i] = src[i];
    if (src[i] == '\0') {
      break;
    }
  }
}

//init_fs
//read

void init_fs() {

  //reserve FS_NPAGES for each inode and set root inode
  for (unsigned i = 0; i < FS_NPAGES; i++) {
    unsigned block = get_free_page();
    if (i == 0) {
      copy_str(RAM_inodes_table[i].name, "/", 1);
      RAM_inodes_table[i].type = FOLDER;
    } else {
      RAM_inodes_table[i].type = EMPTY;
    }

    RAM_inodes_table[i].id = i;
    RAM_inodes_table[i].data_block = block;

    RAM_inodes_table[i].brother = (void*)0;
    RAM_inodes_table[i].father = (void*)0;
    RAM_inodes_table[i].first_son = (void*)0;
    RAM_inodes_table[i].size = 0;
    RAM_inodes_table[i].ref = 0;
  }
}

inode_id alloc_inode(char name[MAX_SIZE_NAME], inode *father, TYPE_FILE type) {

  for (int i = 0; i < FS_NPAGES; i++) {
    if (RAM_inodes_table[i].type == EMPTY) {
      RAM_inodes_table[i].type = type;
      copy_str(RAM_inodes_table[i].name, name, MAX_SIZE_NAME);

      if (father != 0) {
        RAM_inodes_table[i].father = father;
        int brother = find_last_child(father);
        if (brother == -1) {
          father->first_son = &RAM_inodes_table[i];
        } else {
          RAM_inodes_table[brother].brother = &RAM_inodes_table[i];
        }
      } else {
        RAM_inodes_table[i].father = 0;
      }
      return i;
    }
  }
  return -1;
}

int reset_inode(inode_id inode) {
  if (RAM_inodes_table[inode].type != FILE) {
    return -1;
  }

  RAM_inodes_table[inode].size = 0;
  return 0;
}

int write_inode(inode_id inode, const char *data, int size) {
  if (RAM_inodes_table[inode].type != FILE) {
    return -1;
  }
  unsigned long src = (unsigned long)data;
  unsigned long dst =
      RAM_inodes_table[inode].data_block + RAM_inodes_table[inode].size;

  int max_size = PAGE_SIZE - RAM_inodes_table[inode].size;
  if (size > max_size) {
    size = max_size;
  }
  if (size <= 0) {
    return 0;
  }
  int written = memwrite(src, dst, size);

  RAM_inodes_table[inode].size += written;

  return written;
  return memwrite(src, dst, size);
}

int read_inode(inode_id inode, int pos, int size, char* buff_dest) {
  unsigned long size_to_read = size;

  if (inode < 0 || inode >= FS_NPAGES)
    return -1;

  if (RAM_inodes_table[inode].type != FILE)
    return -2;

  if (pos + size >= RAM_inodes_table[inode].size)
    size_to_read = RAM_inodes_table[inode].size - pos;


  unsigned long start_pos = RAM_inodes_table[inode].data_block + pos;
  unsigned long end_pos = start_pos + size;

  if (end_pos >= RAM_inodes_table[inode].data_block + FS_NPAGES)
    size_to_read = (RAM_inodes_table[inode].data_block + FS_NPAGES) - start_pos;
  
  memcpy(start_pos, buff_dest, size_to_read);
  
  return size_to_read;
}


int free_inode(inode_id inode) {
   if (inode < 0 || inode >= FS_NPAGES)
    return -1;

    if (RAM_inodes_table[inode].type == FOLDER && RAM_inodes_table[inode].first_son != (void*)0)
      return -2; //impossible to destroy a folder with element inside
    
    struct RAM_inode* brother = RAM_inodes_table[inode].brother;
    struct RAM_inode* father = RAM_inodes_table[inode].father;
    struct RAM_inode* first_son = RAM_inodes_table[inode].first_son;

    if (father->first_son == &RAM_inodes_table[inode])
      father->first_son = brother;

    struct RAM_inode* next_child = father->first_son;
    while(next_child != (void*)0) {
      if (next_child->brother == &RAM_inodes_table[inode])
        next_child->brother = brother;
        break;
    }

    RAM_inodes_table[inode].brother = (void*)0;
    RAM_inodes_table[inode].father = (void*)0;
    RAM_inodes_table[inode].size = 0;
    RAM_inodes_table[inode].ref = 0;
    RAM_inodes_table[inode].type = EMPTY;

    return 0;
}
