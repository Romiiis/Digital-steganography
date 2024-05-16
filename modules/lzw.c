#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lzw.h"



/**
 * Initializes the dictionary
 * 
 * @param d The dictionary
 * @param index The last index of the dictionary
 * 
 * @return SUCCESS if the dictionary was successfully initialized and FAILURE if something went wrong
*/
int init_d(entry *d, int *index){

    /* Sanity check */
    if (!d || !index) {

        return FAILURE;
    }


    /* Iterate through the dictionary */
    for ((*index) = 0; (*index) < 256; (*index)++) {

        (d)[*index].parent = -1;
        (d)[*index].b = (byte)*index;

    }

    /* Set the last index to 256 */
    *index = 256;

    /* Return success */
    return SUCCESS;
}

/**
 * This function add entry at index 
 * 
 * @param dic The dictionary
 * @param b Byte to add
 * @param parent Index of phrase from what new was created
 * @param index Free index in dictionary 
 * 
 * @return SUCCESS if entry was added, otherwise FAILURE
*/
int add_d(entry *dic, byte b, int parent, int *index){

    /* Sanity check */
    if (!dic || parent >= MAX_DICT_SIZE || !index || *index < 0) {

        return FAILURE;

    }

    /* Add byte and parent in new record */
    dic[*index].b = b;
    dic[*index].parent = parent;

    /* Increment index */
    (*index)++;


    /* If dictionary is full - Reset dictionary */
    if (*index == MAX_DICT_SIZE) {

        /* Reset index */
        *index = 0;

        /* Init new dictionary */
        init_d(dic, index);

    }

    return SUCCESS;



}


/**
 * Checks if a sequence of bytes is in the dictionary
 * 
 * @param dic The dictionary
 * @param b The first byte of the sequence
 * @param parent The parent of the sequence
 * @param index The last index of the dictionary
 * @param found The index of the sequence if it is in the dictionary
 * 
 * @return SUCCESS if the sequence is in the dictionary, FALSE if it is not and FAILURE if something went wrong
 */
int in_dictionary(entry *dic, byte b, int parent ,int index, word *found){

    /* Declare and initialize variables */
    int i;
    *found = FALSE;

    /* Sanity check */
    if (dic == NULL || index < 0) {

        printf("Error in in_dictionary function\n");
        return FAILURE;

    }

    /* Iterate through the dictionary */
    for (i = 0; i < index; i ++) {

        if (b == dic[i].b && dic[i].parent == parent) {

            *found = (word) i;
            return SUCCESS;

        }
    }

    /* If the sequence is not in the dictionary, return 0 */
    return FALSE;
}



/**
 * This function reverse whole block (first byte is in result last)
 * 
 * @param block The address of pointer that points to block
 * @param block_size The size of the block
 * 
 * @return SUCCESS if reverse is completed and FAILURE if something went wrong
*/
int reverse_block(byte **block, int block_size){

    /* Declare and initialize variables */
    byte *temp_block = NULL;
    int i;

    /* Sanity check */
    if (block == NULL || !*block || block_size <= 0) {

        printf("Error in reverse_block function\n");
        return FAILURE;

    }

    /* Allocate memory for the temporary block */
    temp_block = (byte*)malloc(sizeof(byte) * block_size);

    /* Check if the allocation was successful */
    if (!temp_block) {

        return FAILURE;

    }

    /* Copy the block to a temporary block */
    memcpy(temp_block, *block, block_size);

    /* Iterate through the block */
    for (i = 0; i < block_size; i++) {

        (*block)[i] = temp_block[block_size - i - 1];
    }   

    /* Free the temporary block */
    free(temp_block);


    /* Return the size of the block */
    return SUCCESS;


}


/**
 * This function will return block of bytes from dictionary 
 * 
 * @param dic The dictionary
 * @param index The index what phrase you want to get
 * @param block_size In this memory address, the size of block will be stored
 * 
 * @return NULL if something went wrong, otherwise returns data
*/
byte *get_block(entry *dic, int index, int *block_size){

    /* Declare and initialize variables */
    byte *block = NULL;
    int alloc_size = DEFAULT_ALLOC_SIZE,
    block_index = 0;

    /* Sanity check */
    if (dic == NULL || index < 0) {
        printf("Error in get_block function\n");
        return NULL;
    }

    /* Allocate memory for the block */
    block = (byte*)malloc(alloc_size);

    /* Check if the allocation was successful */
    if (!block) {
        return NULL;
    }

    /* Iterate through the dictionary */
    do {   

        /* Add the byte to the block */
        block[block_index++] = dic[index].b;

        if (block_index >= alloc_size) {

            alloc_size += DEFAULT_ALLOC_SIZE;
            block = (byte*)realloc(block, alloc_size);

            /* Check if the reallocation was successful */
            if (!block) {
                return NULL;
            }

        }

        /* Get the parent of the sequence */
        index = dic[index].parent;


    } while(index != -1);


    /* Reverse the block */
    if (!reverse_block(&block, block_index)) {
        return NULL;
    }

    /* Save the size of the block */
    *block_size = block_index;


    /* Return the block */
    return block;


}

/**
 * This function will compress data using LZW 12 bit compression algorithm
 * 
 * @param data Data to compress
 * @param size Size of data 
 * @param size_out At this memory address, output size will be saved
 * 
 * @return NULL if something went wrong, otherwise compressed data 
*/
word *compress(byte *data, int size, int *size_out){

    /* LZW compression using prefix codes */
    /* implementation */

    /* Declare and initialize variables */
    entry dict[MAX_DICT_SIZE];

    int dict_index = 0, i,
    output_index = 0,
    alloc_size = DEFAULT_ALLOC_SIZE,
    exit_code = 0;

    word *output = NULL,
    last = 0,
    index = 0;

    byte code;

    /* Sanity check */
    if (size <= 0 || data == NULL || size_out == NULL) {
        printf("Error in compress function\n");
        return NULL;
    }


    /* Initialize the dictionary */
    exit_code = init_d(dict, &dict_index);
    
    if (exit_code == FAILURE) {
        return NULL;
    }


    /* Allocate memory for the output */
    output = (word*)malloc(sizeof(word) * alloc_size);

    /* Check if the allocation was successful */
    if (!output) {
        printf("Error in compress function\n");
        return NULL;
    }

    /* Get the first byte of the data */
    exit_code = in_dictionary(dict, data[0], -1, dict_index, &last);

    /* Check if there is no FAILURE */
    if (exit_code == FAILURE) {
        printf("Error in compress function\n");
        free(output);
        return NULL;

    }

    /* Iterate through the data */
    for (i = 1; i < size; i++) {

        /* Get the next byte */
        code = data[i];

        /* Check if the sequence is in the dictionary */
        exit_code = in_dictionary(dict, code, last, dict_index, &index);

        if (exit_code == FAILURE) {
            printf("Error in compress function\n");
            free(output);
            return NULL;
        }

        /* If it is, save it as last and continue */
        if (index != 0) {

            last = index;
            continue;
            
        }
        else {

            /* If the sequence is not in the dictionary, add it to the dictionary */

            /* Check if the output array needs to be reallocated */
            if (output_index >= alloc_size) {

                alloc_size += DEFAULT_ALLOC_SIZE;
                output = (word*)realloc(output, sizeof(word) * alloc_size);

                /* Check if the reallocation was successful */
                if (!output) {
                    printf("Error in compress function\n");
                    return NULL;
                }

            }

            /* Output the last code */
            output[output_index++] = last;


            /* Add entry in dictionary */
            exit_code = add_d(dict, code, last, &dict_index);

            if (exit_code == FAILURE) {
                printf("Error in compress function\n");
                free(output);
                return NULL;
            }



            /* Reset the last sequence */
            exit_code = in_dictionary(dict, code, -1, dict_index, &last);

            if (exit_code == FAILURE) {
                printf("Error in compress function\n");
                free(output);
                return NULL;

            }
            
        }


        
    }

    /* Check if the output array needs to be reallocated */
    if (output_index >= alloc_size) {

        alloc_size += DEFAULT_ALLOC_SIZE;
        output = (word*)realloc(output, sizeof(word) * alloc_size);

        /* Check if the reallocation was successful */
        if (!output) {
            printf("Error in compress function\n");
            return NULL;
        }

    }

    /* Output the last sequence */
    output[output_index++] = last;

    /* Save the size of the output array */
    *size_out = output_index;

    /* Reallocate the output array to the correct size */
    output = (word*)realloc(output, sizeof(word) * output_index);

    /* Check if the reallocation was successful */
    if (!output) {
        printf("Error in compress function\n");
        return NULL;
    }

    /* Return the output array */
    return output;
}



/**
 * This function will decompress data using LZW 12 bit decompression algorithm
 * 
 * @param compressed_data Compressed data
 * @param size Size of compressed data array
 * @param size_out At this memory address, output size will be saved
 * 
 * @return NULL if something went wrong, otherwise decompressed data
*/
byte *decompress(word *compressed_data, int size, int *size_out) {


    /* Declare and initialize variables */
    byte *output = NULL, *block = NULL;
    entry dict[MAX_DICT_SIZE];
    int dict_index = 0,
    i,
    alloc_size = DEFAULT_ALLOC_SIZE,
    output_index = 0,
    exit_code = 0,
    block_size = 0,
    last = 0,
    index = 0;



    /* Sanity check */
    if (compressed_data == NULL || size <= 0 || size_out == NULL) {
        printf("Error in d function\n");
        return NULL;
    }

    /* Initialize the dictionary */
    exit_code = init_d(dict, &dict_index);
    
    if (exit_code == FAILURE) {
        return NULL;
    }

    /* Allocate memory for the output */
    output = (byte*)malloc(alloc_size);

    /* Check if the allocation was successful */
    if (!output) {

        return NULL;

    }

    /* Get the first code */
    last = compressed_data[0];

    /* Get the first byte of the sequence */
    output[output_index++] = dict[last].b;





    /* Iterate through the compressed data */
    for (i = 1; i < size; i++) {

        /* Get the next code */
        index = compressed_data[i];

        /* Check if the sequence is in the dictionary */
        if (index < dict_index) {

            /* If it is, get the block */
            block = get_block(dict, index, &block_size);

            /* Check if the block was successfully retrieved */
            if (!block) {
                free(output);
                return NULL;
            }

            /* Check if the output array needs to be reallocated */
            if (output_index + block_size >= alloc_size) {

                alloc_size += DEFAULT_ALLOC_SIZE;
                output = (byte*)realloc(output, alloc_size);

                /* Check if the reallocation was successful */
                if (!output) {
                    return NULL;
                }

            }

            /* Copy the block to the output array */
            memcpy(output + output_index, block, block_size);

            /* Increment the output index */
            output_index += block_size;

            /* Free the block */
            free(block);

            /* Get the first byte of the sequence */
            block = get_block(dict, index, &block_size);

            if (!block) {
                free(output);
                return NULL;
            }


            /* Add entry to dictionary */
            exit_code = add_d(dict, block[0], last, &dict_index);

            free(block);

            if (exit_code == FAILURE ) {
                free(output);
                return NULL;
            }

            /* Save the last index */
            last = index;



        } else if (index == dict_index) {

            /* If the code is equal to the dictionary index, new sequence is last sequence + first byte of last sequence */

            block = get_block(dict, last, &block_size);

            /* Check if the block was successfully retrieved */
            if (!block) {
                free(output);
                return NULL;
            }

            /* First add new sequence to the dictionary */


            exit_code = add_d(dict, block[0], last, &dict_index);

            if (exit_code == FAILURE) {
                free(output);
                return NULL;
            }

            /* Free the block */
            free(block);


            /* Now get the block */
            block = get_block(dict, index, &block_size);

            /* Check if the block was successfully retrieved */
            if (!block) {
                free(output);
                return NULL;
            }

            /* Check if the output array needs to be reallocated */
            if (output_index + block_size >= alloc_size) {

                alloc_size += DEFAULT_ALLOC_SIZE;
                output = (byte*)realloc(output, alloc_size);

                /* Check if the reallocation was successful */
                if (!output) {
                    return NULL;
                }

            }

            /* Copy the block to the output array */
            memcpy(output + output_index, block, block_size);

            /* Increment the output index */
            output_index += block_size;

            /* Free the block */
            free(block);

            /* Save the last index */
            last = index;


        } else {

            printf("Error in decompress\n");
            free(output);
            *size_out = FAILURE;
            return NULL;

        }


    }

    /* Save the size of the output array */
    *size_out = output_index;

    /* Reallocate the output array to the correct size */
    output = (byte*)realloc(output, output_index);

    /* Check if the reallocation was successful */
    if (!output) {
        return NULL;
    }


    /* Return the output array */
    return output;


}



