#include "../include/filesystem.h"



int find_last_child(inode *father) {
    if (father == 0)
        return -1;

    if (!father->first_son)
        return -1;

    inode *cur = father->first_son;
    while (cur->brother)
        cur = cur->brother;

    return cur->id;
}

void copy_str(char *dest, char *src, int size){
  for(int i = 0; i < size; i++){
    dest[i] = src[i];
    if (src[i] == '\0'){
      break;
    }
  }
}


int alloc_inode(char name[MAX_SIZE_NAME], inode* father, TYPE_FILE type){
  
  for(int i =0; i < FS_NPAGES; i++){
    if(RAM_inodes_table[i].type == EMPTY){
      RAM_inodes_table[i].type = type;
      copy_str(RAM_inodes_table[i].name, name, MAX_SIZE_NAME);
      
      if(father != 0){
        RAM_inodes_table[i].father = father;
        int brother = find_last_child(father);
        if(brother ==-1){
          father->first_son = &RAM_inodes_table[i];
        }else{
          RAM_inodes_table[brother].brother = &RAM_inodes_table[i];
        }
      }else{
        RAM_inodes_table[i].father = 0;
      }
      return i;
    }
  }
  return -1;
}


int reset_inode(int inode){  
  if (RAM_inodes_table[inode].type != FILE) {
    return -1;
  }

  RAM_inodes_table[inode].size = 0;
  return 0;
}
