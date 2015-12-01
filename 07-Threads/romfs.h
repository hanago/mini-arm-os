#ifndef __ROMFS_H__
#define __ROMFS_H__

#include <stdint.h>

typedef struct romf romf;

int init_romfs();

uint32_t findnode_romfs(char *localPath);

romf* fopen_romfs(char* path);
char  fread_c_romfs(romf* fp);
char*  fread_str_romfs(romf* fp);
int   fclose_romfs(romf* fp);

int show_dirlist_romfs(char* path);

#endif
