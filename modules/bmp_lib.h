/* BMP_LIB.H */

/* Inclusion guard */
#ifndef __BMP_LIB_H__
#define __BMP_LIB_H__


#include "my_defs.h"


/* Defines */
#define BIT_PER_PIXEL 24
#define BMP_TYPE "BM"
#define BMP_HEADER_SIZE 54
#define ALIGN 4


/* Structures */

/* This structure represents pixel */
typedef struct {

    /* Red */
    byte r;

    /* Green */
    byte g;

    /* Blue */
    byte b;

} pixel;

typedef struct {

    /* BMP HEADER */
    char id_field_1;
    char id_field_2;
    int size_of_bmp;
    int unused;
    int offset;

    /* DIB */
    int size_of_dib;
    int width;
    int height;
    short planes;
    short bits_per_pixel;
    int compression;
    int size_of_raw;
    int horizontal_res;
    int vertical_res;
    int colors;
    int imp_colors;
    char *path;


}  __attribute__((__packed__)) BMP_HEAD;



/**
 * This function frees the BMP_data structure.
 * 
 * @param bmp_data Pointer to the BMP_data structure
 * 
 * @return void
*/
void free_bmp_header(BMP_HEAD *bmp_header);


/**
 * This function writes the BMP file.
 * 
 * @param png_row_pointers Array of png_bytep (pixels of image)
 * @param bmp_data Pointer to the BMP_data structure
 * 
 * @return SUCCESS or FAILURE
*/
int write_bmp(png_bytep **png_row_pointers, BMP_HEAD *bmp_header);

/**
 * This function process the BMP file (hide / extract data).
 * 
 * @param paths Array of paths to files
 * @param sw Switch
 * 
 * @return return code (0 if success, 2 not in correct format, 3 if error)
*/
int proceed_bmp(char **paths, char sw);



#endif