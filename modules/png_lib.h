/* PNG_LIB.H */

/* Inclusion guard */
#ifndef __PNG_LIB_H__
#define __PNG_LIB_H__

#include "my_defs.h"





/* Prototypes */

/**
 * This function will write PNG file
 * 
 * @param filename Name of the file
 * @param width Width of image
 * @param height Height of image
 * @param row_pointers Pointer to array of pointers to rows of pixels
 * @param read_png Pointer to the png_structp
 * @param read_info Pointer to the png_infop
 * 
 * @return SUCCESS or FAILURE
*/
int write_png_file(char *filename, png_bytep *row_pointers, png_structp read_png, png_infop read_info);


/**
 * This function will read PNG file
 * 
 * @param filename Name of the file
 * @param width Pointer to the width of image
 * @param height Pointer to the height of image
 * @param row_pointers Pointer to array of pointers to rows of pixels
 * @param png Pointer to the png_structp
 * @param info Pointer to the png_infop
 * 
*/
int read_png(char *filename, int *width, int *height, png_bytep **row_pointers, png_structp *png, png_infop *info);


/**
 * This function will hide / extract data in / from PNG file
 * 
 * @param paths Array of paths to files
 * @param sw Switch
 * 
 * @return 0 success, 2 not in correct format, 3 image too small, 4 no hidden content , 5 damagged file, 6 error
*/
int proceed_png(char **paths, char sw);

#endif