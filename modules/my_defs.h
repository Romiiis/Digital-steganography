/* MY_DEFS.H */


/* This header file contains definitions for the types used in the whole program. */

#ifndef __MY_DEFS_H__
#define __MY_DEFS_H__



#define SUCCESS 1
#define FAILURE -1

#define TRUE 1
#define FALSE 0


/* Types */
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;


/* Forward declarations for libpng */
typedef struct png_struct_def png_struct_def;
typedef png_struct_def* png_structp;
typedef struct png_info_def png_info_def;
typedef png_info_def* png_infop;
typedef unsigned char png_byte;
typedef png_byte* png_bytep;


#endif