/* PIXEL_SECRETS.H */

/* Inclusion guard */
#ifndef __CONVERTOR_H__
#define __CONVERTOR_H__

#include "my_defs.h"


/* Defines */
#define mask_1 0x01
#define mask_0 0xFE

#define BYTES_PER_PIXEL 3
#define COLUMN_START 2

/* CRC32 defines */

#define CRC32_POLYNOMIAL 0x04C11DB7u
#define CRC32_SIZE 32

/* Watermark defines */
#define WATERMARK_SIZE 16
#define WATERMARK "hD"



/* Prototypes */

/**
 * This function will free the array of pointers to rows of pixels
 * 
 * @param row_pointers Array of pointers to rows of pixels
 * @param row_count Number of rows
 * 
 * @return void
*/
void free_row_pointers(png_bytep *row_pointers, int row_count);


/**
 * This function calculates the CRC32 checksum of the data.
 * 
 * @param message Data to be calculated
 * @param length Size of the data
 * 
 * @return CRC32 checksum
 * 
 */
dword crc32b(word *message, int length);


/**
 * This function will hide the data in the image. (PNG or BMP)
 * 
 * @param width The width of the image
 * @param height The height of the image
 * @param row_pointers Array of pointers to rows of pixels
 * @param payload_path Path to the file to be hidden
 * @return 0 if success, 3 if bmp file is not big enough, 6 if other error
 */
int hide_in_image(int width, int height, png_bytep **row_pointers, char *payload_path);


/**
 * This function will extract the data from the image. (PNG or BMP)
 * 
 * @param width The width of the image
 * @param row_pointers Array of pointers to rows of pixels
 * @param to Path to the file to be extracted
 * @return 0 if success, 4 if no hidden content, 5 if invalid crc32, 6 if other error
 */
int extract_from_image(int width, png_bytep **row_pointers, char *to);

#endif