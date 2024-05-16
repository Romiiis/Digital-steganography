/* PIXEL_SECRETS.C */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include "lzw.h"
#include "pixel_secrets.h"
#include "input.h"


/**
 * This function frees the memory allocated for the row_pointers.
 * 
 * @param row_pointers Array of png_bytep
 * @param row_count Number of rows
 * 
 * @return void
*/
void free_row_pointers(png_bytep *row_pointers, int row_count){

    /* Declaration variables */
    int i;

    /* Sanity check */
    if (!row_pointers || row_count < 0) {
        printf("Error in free_row_pointers\n");
        return;
    }

    for (i = 0 ; i < row_count ; i++) {

        if (row_pointers[i]) free(row_pointers[i]);
        
    }

    free(row_pointers);

}



/**
 * This function hides the compressed data in the pixels in BLUE channel (LSB).
 * 
 * @param compressed Array of words
 * @param compressed_size Size of the compressed data
 * @param row_pointers_pt Pointer to the array of png_bytep
 * @param width Width of the picture
 * @param height Height of the picture
 * 
 * @return SUCCESS if success, FAILURE if error
*/
int hide_mechanism(word *compressed, int compressed_size, png_bytep **row_pointers_pt, int width, int height){

    /* Declaration of variables */
	int i, j, w_row_end = 0, w_col_end = COLUMN_START;
    dword crc32;
    char *watermark = WATERMARK;
    png_bytep *row_pointers = *row_pointers_pt;


	/* Check if the picture file is big enough */
	if ((width * height) < (compressed_size * COMPRESSED_SIZE) + WATERMARK_SIZE + CRC32_SIZE + ((int)sizeof(int) * 8)) {
		printf("Picture file is too small!\n");
		return 1;
	}


     /* Write the watermark */
    for (i = 0; i < 2; i++) {

        for (j = (sizeof(char) * 8) - 1 ; j >= 0; j--) {

            if (w_col_end == ( ((width * BYTES_PER_PIXEL) - 1) + BYTES_PER_PIXEL) ) {
                w_col_end = COLUMN_START;
                w_row_end++;
            }
            
            if ((watermark[i] >> j) & 1) {
                row_pointers[w_row_end][w_col_end] |= mask_1;


            }
            else {
                row_pointers[w_row_end][w_col_end] &= mask_0;
            }

            w_col_end += BYTES_PER_PIXEL;

        }
    }

    /* Write the size of the compressed data */
    for (i = (sizeof(int) * 8 ) - 1; i >= 0; i--) {

        if ((w_col_end == (((width * BYTES_PER_PIXEL) - 1) + BYTES_PER_PIXEL))) {
            w_col_end = COLUMN_START;
            w_row_end++;
        }
        if ((compressed_size >> i) & 1) {
            row_pointers[w_row_end][w_col_end] |= mask_1;
        }
        else {
            row_pointers[w_row_end][w_col_end] &= mask_0;
        }

        w_col_end += BYTES_PER_PIXEL;
       
    }


    /* Write the compressed data */
    for (i = 0; i < compressed_size; i++) {

        for (j = COMPRESSED_SIZE - 1; j >= 0; j--) {

            if ((w_col_end == (((width * BYTES_PER_PIXEL) - 1) + BYTES_PER_PIXEL))) {
                w_col_end = COLUMN_START;
                w_row_end++;
            }
            
            if ((compressed[i] >> j) & 1) {
                row_pointers[w_row_end][w_col_end] |= mask_1;
            }
            else {
                row_pointers[w_row_end][w_col_end] &= mask_0;
            }

            w_col_end += BYTES_PER_PIXEL;

        }
    }


    /* Write the crc32 */
    crc32 = crc32b(compressed, compressed_size);

    for (i = CRC32_SIZE- 1; i >= 0; i--) {
        if ((w_col_end == (((width * BYTES_PER_PIXEL) - 1) + BYTES_PER_PIXEL))) {
            w_col_end = COLUMN_START;
            w_row_end++;
        }
        if ((crc32 >> i) & 1) {

            row_pointers[w_row_end][w_col_end] |= mask_1;
        }
        else {
            row_pointers[w_row_end][w_col_end] &= mask_0;
        }

        w_col_end += BYTES_PER_PIXEL;
       
    }


    return 0;
}

/**
 * This function extracts the compressed data in the pixels in BLUE channel (LSB).
 * 
 * @param row_pointers_pt Pointer to the array of png_bytep 
 * @param size Size of the extracted data
 * @param width Width of the picture
 * @param height Height of the picture
 * @param returns Code of return to check what happened (SUCCESS, FAILURE, 4 - NO HIDDEN CONTENT, 5 - INVALID CRC32)
 * 
 * @return NULL if error, otherwise extracted data
*/
word *extract_mechanism(png_bytep **row_pointers_pt, int *size ,int width, int *returns){

    /* Declaration and initialization of variables */
	int i, j,
    w_row_end = 0,
    w_col_end = COLUMN_START,
    w_size = 0;

    word *compressed = NULL, temp = 0;

	png_bytep *row_pointers = *row_pointers_pt;

    dword crc32, w_crc32 = 0;

    char *watermark;


    /* Allocate memory for watermark */
    watermark = (char *)malloc(sizeof(char) * 3);

    if (!watermark) {
        printf("Error in extract_mechanism!\n");
        *returns = FAILURE;
        return NULL;
    }


    watermark[2] = '\0';



    /* Read the watermark */
    for (i = 0; i < 2; i++) {

        for (j = (sizeof(char)*8) - 1 ; j >= 0; j--) {

            if ((w_col_end == (((width * BYTES_PER_PIXEL) - 1) + BYTES_PER_PIXEL))) {
                w_col_end = COLUMN_START;
                w_row_end++;
            }
            
            if (row_pointers[w_row_end][w_col_end] & mask_1) {
                watermark[i] |= 1 << j;
            }
            else {
                watermark[i] &= ~(1 << j);
            }

            w_col_end += BYTES_PER_PIXEL;

        }
    }

    if (strcmp(watermark, WATERMARK)) {

        free(watermark);

        /* NO HIDDEN CONTENT - 4 */
        *returns = 4;
        return NULL;
    }


    /* Free watermark */
    free(watermark);

    /* Read the size of the compressed data */
    for (i = (sizeof(int) * 8 ) - 1; i >= 0; i--) {

        if ((w_col_end == (((width * BYTES_PER_PIXEL) - 1) + BYTES_PER_PIXEL))) {
            w_col_end = COLUMN_START;
            w_row_end++;
        }
        
        if (row_pointers[w_row_end][w_col_end] & mask_1) {
            w_size |= 1 << i;
        }
        else {
            w_size &= ~(1 << i);
        }

        w_col_end += BYTES_PER_PIXEL;
       
    }

    compressed = (word *)malloc(sizeof(word) * w_size);
	*size = w_size;

    if (compressed == NULL) {
        printf("Error in extract_mechanism!\n");
        *returns = FAILURE;
        return NULL;
    }

    /* Read the compressed data */
    for (i = 0; i < w_size; i++) {

        temp = 0;
        for (j = COMPRESSED_SIZE - 1; j >= 0; j--) {

            if ((w_col_end == (((width * BYTES_PER_PIXEL) - 1) + BYTES_PER_PIXEL))) {
                w_col_end = COLUMN_START;
                w_row_end++;
            }
            
            if (row_pointers[w_row_end][w_col_end] & mask_1) {
                temp |= 1 << j;
            }
            else {
                temp &= ~(1 << j);
            }

            w_col_end += BYTES_PER_PIXEL;

        }

        compressed[i] = temp;
    }


    /* Read the crc32 */
    for (i = CRC32_SIZE- 1; i >= 0; i--) {

        if ((w_col_end == (((width * BYTES_PER_PIXEL) - 1) + BYTES_PER_PIXEL))) {
            w_col_end = COLUMN_START;
            w_row_end++;
        }
        
        if (row_pointers[w_row_end][w_col_end] & mask_1) {
            w_crc32 |= 1 << i;
        }
        else {
            w_crc32 &= ~(1 << i);
        }

        w_col_end += BYTES_PER_PIXEL;
       
    }

    crc32 = crc32b(compressed, w_size);

    if (crc32 != w_crc32) {
        /* INVALID CRC32, CONTENT DAMAGED - 5 */
        free(compressed);
        *returns = 5;
        return NULL;
    }


    *returns = SUCCESS;


    return compressed;
}


/**
 * This function calculates the crc32 of a message.
 * @param message The message to calculate the crc32.
 * @param length The length of the message.
 * 
 * @return The crc32 of the message if the message was calculated successfully and FAILURE if an error occurred.
*/
dword crc32b(word *message, int length) {

	/* Declare and initialize variables */
   	int i = 0, j;
   	unsigned int current_word, crc = 0xFFFFFFFF, mask;

	/* Sanity check */
	if (!message || length <= 0) {
		printf("Error in crc32b function\n");
		return FAILURE;
	}

	/* Iterate through message */
	while (i < length) {

		/* Get current word */
		current_word = message[i]; 

		/* XOR crc with current word */
		crc = crc ^ current_word;

		/* Iterate through current word */
		for (j = 15; j >= 0; j--) { 

			/* Get mask */
			mask = -(crc & 1);

			/* Shift crc */
			crc = (crc >> 1) ^ (CRC32_POLYNOMIAL & mask);
		}

		/* Increase i */
		i = i + 1;
	}


	/* Return crc */
	return ~crc;


}


/**
 * This function hides the compressed data in the picture.
 * 
 * @param width Width of the picture
 * @param height Height of the picture
 * @param row_pointers_pt Pointer to the array of png_bytep
 * @param payload_path Path to the payload
 * 
 * @return 0 if success, 3 if bmp file is not big enough, 6 if other error
*/
int hide_in_image(int width, int height, png_bytep **row_pointers, char *payload_path){

    /* Declaration of variables */
    word *compressed = NULL;
    payload *data = NULL;
    int size = 0;
    
    /* Read payLoad file */
    data = get_payload(payload_path);

    if (!data) {
        printf("Error in hide_in_image!\n");

        /* OTHER ERROR - 6 */
        return 6;
    }

    /* Compress data */
    compressed = compress(data->data, data->size, &size);

    if (!compressed) {
        printf("Error in hide_in_image!\n");

        /* OTHER ERROR - 6 */
        return 6;
    }


    /* Check if the bmp file is big enough */
    if ((width * height) >= (size  * COMPRESSED_SIZE) + WATERMARK_SIZE + CRC32_SIZE + ( (int) sizeof(int) ) * 8) {

        
        printf("Hiding data ...\n");

        /* Hide the data */
        if (hide_mechanism(compressed, size, row_pointers, width, height) != FAILURE) {

            printf("Data hidden successfully!\n");
            free_payload(data);
            free(compressed);

            /* DATA HIDDEN - 0 */
            return 0;

        } else {

            printf("Data hiding went wrong!\n");
            free_payload(data);
            free(compressed);

            /* DIFFERENT ERROR - 6 */
            return 6;

        }


    } else {

        /* Inform user that the bmp file is not big enough */
        printf("Data is too big to hide in this picture!\nPlease choose a bigger picture!\n");
        /* Free memory */
        free_payload(data);
        free(compressed);

        /* BMP FILE IS NOT BIG ENOUGH - 3 */
        return 3;

    }

}


/**
 * This function extracts the compressed data from the picture.
 * 
 * @param width Width of the picture
 * @param height Height of the picture
 * @param row_pointers Pointer to the array of png_bytep
 * @param to Path to the file where the data will be written
 * 
 * @return 0 if success, 4 if no hidden content, 5 if invalid crc32, 6 if other error
*/
int extract_from_image(int width, png_bytep **row_pointers, char *to){

    /* Declaration and  of variables */
	int size = 0, ex_ret = 0, str_size = 0;
	word *compressed = NULL;
	byte *decompressed = NULL;
    FILE *file = NULL;
    

	/* Extract the data from image */
	compressed = extract_mechanism(row_pointers, &size, width, &ex_ret);

    /* Check what happened */
    switch (ex_ret) {
        case FAILURE: {

            /* DIFFERENT ERROR - 6 */
            return 6;

        };
        case 4: {
            
            /* Free memory */
            free(compressed);
            /* NO HIDDEN CONTENT - 4 */
            printf("No hidden content!\nTry a different picture!\n");
            return 4;

        };
        case 5: {
            /* FREE Memory */
            free(compressed);
            /* INVALID CRC32 - 5 */
            printf("Invalid crc32!\nContent damaged!\n");
            return 5;

        };

    }

    /* Check if the data was extracted */
	if(!compressed){

		/* Nothing to free */ 
		printf("Failed to extract data!\n");

        /* DIFFERENT ERROR - 6 */
		return 6;
	}

    


	/* Decompress the data */
	decompressed = decompress(compressed, size, &str_size);

	/* Check if the data was decompressed */
	if (!decompressed && str_size == FAILURE) {

		/* Free compressed */
		free(compressed);

		printf("Failed to decompress data!\n");

        /* FAILED TO DECOMPRESS DATA - 5*/
		return 5;

	} else if (!decompressed) {
            
        /* Free compressed */
        free(compressed);

        /* DIFFERENT ERROR - 6 */
        return 6;

    } else {

        printf("Content extracted successfully!\n");

    }

    /* Free compressed - dont needed anymore */
    free(compressed);


	/* Write the data to the file */
	file = fopen(to, "wb");

	/* Check if the file was opened */
	if (!file) {

		free(decompressed);
		printf("Failed to open the file");

        /* DIFFERENT ERROR - 6 */
		return 6;
	}

	/* Write the data to the file */
	if (fwrite(decompressed, 1, str_size, file) != (size_t)str_size) {

		free(decompressed);
		printf("Failed to write to the file");
            
        /* DIFFERENT ERROR - 6 */
		return 6;
	}

    /* Inform that it was write good*/
    printf("Data written successfully in %s!\n", to);

	/* Free memory */
	free(decompressed);
	
	/* Close the file */
	fclose(file);

    /* Inform user that the data was extracted successfully */
	return 0;
}



