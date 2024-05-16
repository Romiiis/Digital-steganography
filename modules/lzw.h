/* L.H*/

/* Inclusion guard */
#ifndef __L_H__
#define __L_H__

#include "my_defs.h"

/* Defines */
#define MAX_DICT_SIZE 4096
#define INIT_DICT_SIZE 256
#define DEFAULT_ALLOC_SIZE 512
#define COMPRESSED_SIZE 12



/* Structures */
typedef struct{

    int parent;
    byte b;

}entry;

/* Prototypes */

/**
 * This function will compress data with LZW compression (12bit)
 * 
 * @param data Data to be compressed
 * @param size Size of data (array)
 * @param size_out Size of returned array
 * 
 * @return NULL if something went wrong. otherwise compressed data
*/
word *compress(byte *data, int size, int *size_out);


/**
 * This function will decompress data with LZW decompression (12bit)
 * 
 * @param compressed_data Compressed data using same algorithm 
 * @param size Size of compressed data (array)
 * @param size_out Size of returned array
 * 
 * @return NULL if something went wrong, otherwise decompressed data (array)
*/
byte *decompress(word *compressed_data, int size, int *size_out);



#endif