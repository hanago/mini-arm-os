#include "romfs.h"

#include <stddef.h>
#include <ctype.h>

#include "string.h"
#include "usart.h"
#include "malloc.h"

#define ROMFS_BEGIN 0X08010000

static uint32_t  Romfs_size;
static char*     Romfs_name;

/** romfs encoding formate (reference to genromfs.c) **/
/* 16 bytes romfs header & 16 bytes for name*/
#define OFFSET_NEXTFH   0X00
	#define ROMFH_MASK_NEXTFH   0XFFFFFFF0
    #define ROMFH_MASK_TYPE	  	0X00000007
    #define ROMFH_MASK_EXE      0X00000008
#define OFFSET_INFO     0X04
	#define ROMFH_HRD  0 // link destination [file header]
	#define ROMFH_DIR  1 // first file's header
	#define ROMFH_REG  2 // unused, must be zero [MBZ]
	#define ROMFH_LNK  3 // unused, MBZ (file data is the link content)
	#define ROMFH_BLK  4 // 16/16 bits major/minor number
	#define ROMFH_CHR  5 // 	    - " -
	#define ROMFH_SCK  6 // unused, MBZ
	#define ROMFH_FIF  7 // unused, MBZ
#define OFFSET_SIZE     0X08
#define OFFSET_CHECKSUM 0X0C
#define OFFSET_NAME     0X10

#define OFFSET_FH       0X20

#ifdef ROMFS_BigEndian 
#define ROMFH_NEXTFH(fh)  	  (*((uint32_t*)(fh+OFFSET_NEXTFH)) & ROMFH_MASK_NEXTFH)
#define ROMFH_INFO(fh)		  *((uint32_t*)(fh+OFFSET_INFO))
#define ROMFH_TYPE(fh)        (int)((*(uint32_t*)(fh+OFFSET_NEXTFH)) & ROMFH_MASK_TYPE)
#define ROMFH_EXECUTABLE(fh)  (((fh+OFFSET_NEXTFH) & ROMFH_MASK_EXE) >> 4)
#define ROMFH_SIZE(fh) 		  *((uint32_t*)(fh+OFFSET_SIZE))
#else //LittleEndian (use the by default)
#define NTOHL(n) (((((uint32_t)(n) & 0xFF)) << 24) | \
                  ((((uint32_t)(n) & 0xFF00)) << 8) | \
                  ((((uint32_t)(n) & 0xFF0000)) >> 8) | \
                  ((((uint32_t)(n) & 0xFF000000)) >> 24))
#define ROMFH_NEXTFH(fh)  	  ( NTOHL(*(uint32_t*)(fh+OFFSET_NEXTFH)) & ROMFH_MASK_NEXTFH )
#define ROMFH_INFO(fh)		  ( NTOHL(*(uint32_t*)(fh+OFFSET_INFO)) )
#define ROMFH_TYPE(fh)        ( (int)(NTOHL(*(uint32_t*)(fh+OFFSET_NEXTFH)) & ROMFH_MASK_TYPE) )
#define ROMFH_EXECUTABLE(fh)  ( (int)(NTOHL(*(uint32_t*)(fh+OFFSET_NEXTFH)) & ROMFH_MASK_EXE >> 4) )
#define ROMFH_SIZE(fh) 		  ( NTOHL(*((uint32_t*)(fh+OFFSET_SIZE))) )
#endif

/* romfs internal data types */

typedef struct romf {
	uint32_t addr;
	int executable;
	uint32_t size;
	char* name;
	char* sdata;
	char* ptr;
} romf;


/* general operation */
int init_romfs(){
	Romfs_name = (char*)ROMFS_BEGIN;

    if(strcmp(Romfs_name, "-rom1fs-"))
    	return 0;

    Romfs_size = *((uint32_t* )(ROMFS_BEGIN + OFFSET_SIZE));
    return 1;
}

uint32_t findnode_romfs(char *abs_path)
{
	//Complexity: o(n)
	uint32_t addr;
	char *addr_name, *tmp;
	char buffer[20];//*be care of overflow of string
	int isexist, islast, i, nextPath_ptr;
    
    isexist = 0;
    islast = 0;

    addr = ROMFS_BEGIN+OFFSET_FH;
    addr_name = (char*) (addr+OFFSET_NAME);
    do{
    	/* 0. It shall be a dictory. */
    	if(ROMFH_TYPE(addr) != ROMFH_DIR)
    		break;

    	/* 1. Get name of filenode. */
		// ignore all '/' at begining place
		while(*(abs_path) == '/') 
			abs_path++;
        
        //copy file node name and find next '/' 
        i = 0;
        nextPath_ptr = 0;
        while(*(abs_path+nextPath_ptr) && 
        	  *(abs_path+nextPath_ptr) != '/'){
        	buffer[i++] = *(abs_path+nextPath_ptr);
        	nextPath_ptr++;
        }
        buffer[i] = '\0';

        //check whether it's last file node in the path
		for(tmp = abs_path+nextPath_ptr; (*tmp) && ((*tmp) == '/');tmp++);
		islast = (*tmp) ? 0 : 1;

		/* 2. Check whether it's exist or not. */
		addr = ROMFS_BEGIN + ROMFH_INFO(addr);
		isexist = 0;
	    do{
	    	addr_name = (char*) (addr+OFFSET_NAME);
	    	if(!strcmp(buffer, addr_name)){
	    		isexist = 1;
	    		break;
	    	}
	    	if(!ROMFH_NEXTFH(addr))
	    		break;
	    	addr = ROMFS_BEGIN + ROMFH_NEXTFH(addr);
	    }while(addr);

	    if((!isexist) || islast)
	    	break;

	    /* 3. Go deeper. */
		abs_path += nextPath_ptr+1;
	}while(*(abs_path));

	return isexist && islast? addr : 0;
}

/* file operation */
romf* fopen_romfs(char* path){
	uint32_t addr;

	addr = findnode_romfs(path);
	if(!addr || 
	   ROMFH_TYPE(addr) != ROMFH_REG )
		return NULL;

	romf* newf = (romf*) malloc(sizeof(romf));
    newf->addr = addr;
    newf->executable = ROMFH_EXECUTABLE(addr) ? 1 : 0;
	newf->size = ROMFH_SIZE(addr);
	newf->name = (char*)(addr+0x10);
	newf->sdata = (char*)(addr+0x20);
	newf->ptr = newf->sdata;

	return newf;
}

char fread_c_romfs(romf* fp){
	if(!fp)
		return 0;

	if(fp->ptr+1 > fp->size+fp->sdata)
	    return 0;

	return *(fp->ptr++);
}

char* fread_str_romfs(romf* fp){
	if(!fp)
		return 0;

	char* str = fp->ptr;
	while((*fp->ptr) && ((*fp->ptr) != '\n'))
		fp->ptr++;

	return str;
}

int fclose_romfs(romf* fp){
	if(!fp)
		return 0;
     free((void*)fp);
     return 1;
}

/* directory operation */
/** Require "usart.h" for print. **/
int show_dirlist_romfs(char* path){
    uint32_t cursor;
    int type;
    
    /* 1. Make sure the path is exist. */
    cursor = findnode_romfs(path);
    if(!cursor)
    	return 0;
    
    /* 2. If it's a link, link to destination */
    type = ROMFH_TYPE(cursor);
    if(type == ROMFH_HRD)
    	while(ROMFH_TYPE(cursor) == ROMFH_HRD)
    		cursor = ROMFS_BEGIN + 
    	            ROMFH_INFO(cursor);

   /* 3. Make sure thar it's a dictionary. */
    type = ROMFH_TYPE(cursor);
    if(type != ROMFH_DIR)
    	return 0;

    /* 4. List all files in the dictionary by linking list structure. */
    cursor = ROMFS_BEGIN + ROMFH_INFO(cursor);
    int count = 0;
    do{
    	print_str((char*)(cursor+OFFSET_NAME));
    	//get address of next file header
    	if(!ROMFH_NEXTFH(cursor))
    		break;
    	cursor = ROMFS_BEGIN + ROMFH_NEXTFH(cursor);
    	count++;
    	if(count%5)
    		print_c('\t');
        else
        	print_c('\n');
    }while(cursor);
    if(count%5)
    	print_c('\n');

    return 1;
}