#ifndef _MYTYPE_H_
#define _MYTYPE_H_

//加入类型相关头文件 比如AVR GCC中的 ROM类型在<AVR/pgmspace.h>中定义

#define UINT8   unsigned char
#define UINT16  unsigned int
#define UINT32  unsigned long 

#define INT8    char 
#define INT16   int
#define INT32   long

#define ROM_TYPE_UINT8  unsigned char code 
#define ROM_TYPE_UINT16 unsigned int code 
#define ROM_TYPE_UINT32 unsigned long code 

#endif
