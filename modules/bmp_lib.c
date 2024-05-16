/* BMP_READER.C */
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "bmp_lib.h"
#include "pixel_secrets.h"


/**
 * This function writes the BMP file.
 * 
 * @param filename Name of the file
 * @param png_row_pointers Array of png_bytep
 * @param bmp_data Pointer to the BMP_data structure
 * 
 * @return SUCCESS or FAILURE
*/
int write_bmp(png_bytep **png_row_pointers, BMP_HEAD *bmp_header) {

    /* Declaration and initialization of variables */
    FILE *fp;
    png_bytep *row_pointers;
    int align = 0,
    i, j;
    byte b;

    /* Sanity check */
    if (!bmp_header || !png_row_pointers) {
        printf("Error in write_bmp!\n");
        return FAILURE;
    }

    /* Get the png_row_pointers */  
    row_pointers = *png_row_pointers;

    /* Open the file */
    fp = fopen(bmp_header->path , "wb");

    /* Check if the file was opened */
    if (!fp) {

        printf("Error in write_bmp!\n");
        return FAILURE;
    }


    /* Write header */
    fwrite(bmp_header, 1, bmp_header->offset, fp);

    /* Calculate align bytes */
    align = (ALIGN - ( (bmp_header->width * sizeof(pixel) ) % ALIGN) ) % ALIGN;



    /* Write pixel data */
    for (i = 0; i < bmp_header->height; i++) {

        for (j = 0; j < bmp_header->width; j++) {
                
            /* Write pixel data b, g,r */
            b = row_pointers[i][3 * j + 2];
            fwrite(&b, sizeof(byte), 1, fp);
            b = row_pointers[i][3 * j + 1];
            fwrite(&b, sizeof(byte), 1, fp);
            b = row_pointers[i][3 * j];
            fwrite(&b, sizeof(byte), 1, fp);
        }

        /* Write align bytes */
        for (j = 0; j < align; j++) {
            fwrite("\0", 1, 1, fp);
        }

    }

    /* Close the file */
    fclose(fp);

    return 0;




}



/**
 * This function reads the BMP file.
 * 
 * @param bmp_data Pointer to the BMP_data structure
 * @param png_row_pointers Array of png_bytep
 * 
 * @return SUCCESS or FAILURE
 * 
*/
int read_bmp(BMP_HEAD *bmp_header, png_bytep **png_row_pointers){

    /* Declaration and initialization of variables */
    FILE *fp = NULL;
    int i, j, align = 0;
    pixel **bmp_row_pointers = NULL;
    byte b;

    /* Sanity check */
    if (!bmp_header || !png_row_pointers) {
        return FAILURE;
    }

    /* Open the file */
    fp = fopen(bmp_header->path, "rb");

    /* Check if the file was opened */
    if (!fp) {
        printf("Error in read_bmp!\n");
        return FAILURE;
    }

    /* Move the pointer to the beginning of the pixel data */
    fseek(fp, bmp_header->offset, SEEK_SET); 

    /* Calculate align bytes */
    align = (ALIGN - ( (bmp_header->width * sizeof(pixel) ) % ALIGN) ) % ALIGN;

    /* Allocate memory for BMP pixel data */
    bmp_row_pointers = (pixel **) malloc(sizeof(pixel *) * (bmp_header->height));

    /* Check if the memory was allocated */
    if (!bmp_row_pointers) {
        fclose(fp);
        printf("Error in read_bmp!\n");
        return FAILURE;
    }


    /* Read pixel data */
    for (i = 0; i < bmp_header->height; i++) {

        /* Allocate memory for the row */
        bmp_row_pointers[i] = (pixel *)malloc(sizeof(pixel) * (bmp_header->width));

        /* Check if the memory was allocated */
        if (!bmp_row_pointers[i]) {

            fclose(fp);
            printf("Error in read_bmp!\n");
            return FAILURE;
        }

        for (j = 0; j < bmp_header->width; j++) {

            /* Read pixel data b,g,r */
            fread(&b, sizeof(byte), 1, fp);
            (bmp_row_pointers[i][j]).b = b;
            fread(&b, sizeof(byte), 1, fp);
            (bmp_row_pointers[i][j]).g = b;
            fread(&b, sizeof(byte), 1, fp);
            (bmp_row_pointers[i][j]).r = b;



        }

        /* Skip align bytes */
        fseek(fp, align, SEEK_CUR);
    }


    fclose(fp);


    /* Convert data in png_bytep */

    /* Allocate memory for PNG pixel data */
    *png_row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * (bmp_header->height));

    /* Check if the memory was allocated */
    if (!(*png_row_pointers)) {
        printf("Error in read_bmp!\n");
        return FAILURE;
    }

    /* Convert data */
    for (i = 0; i < bmp_header->height; i++) {
        
        /* Allocate memory for the row */
        (*png_row_pointers)[i] = (png_byte *)malloc(3 * (bmp_header->width));

        /* Check if the memory was allocated */
        if (!(*png_row_pointers)[i]) {
            printf("Error in read_bmp!\n");
            return FAILURE;
        }

        for (j = 0; j < bmp_header->width; j++) {

            (*png_row_pointers)[i][3 * j] = bmp_row_pointers[i][j].r;
            (*png_row_pointers)[i][3 * j + 1] = bmp_row_pointers[i][j].g;
            (*png_row_pointers)[i][3 * j + 2] = bmp_row_pointers[i][j].b;

        }

    }


    /* Free the pixels */
    for (i = 0; i < bmp_header->height; i++) {
        free(bmp_row_pointers[i]);
    }

    /* Free the rows */
    free(bmp_row_pointers);

    return SUCCESS;
}


/**
 * This function frees the memory allocated for the BMP_data structure.
 * 
 * @param bmp_data Pointer to the BMP_data structure
 * 
 * @return void
 * 
*/
void free_bmp_header(BMP_HEAD *bmp_header){
    
    /* Sanity check */
    if (!bmp_header) {
        printf("Error in free_bmp_data!\n");
        return;
    }
    
    /* Free the path */
    free(bmp_header);

    /* Set the pointer to NULL */
    bmp_header = NULL;
    
}


/**
 * This checks the BMP file and returns the BMP_data structure.
 * 
 * @param path Path to the file
 * 
 * @return Pointer to the BMP_data structure or NULL if error
*/
BMP_HEAD *check_bmp(char *path){

    /* Declaration and initialization of variables */

    BMP_HEAD *bmp_header = NULL;
    FILE *file;


    /* Sanity check */
    if (!path) {
        printf("Error in check_bmp!\n");
        return NULL;
    }

    /* Allocate memory for the structure */
    bmp_header = malloc(sizeof(BMP_HEAD));

    /* Check if the allocation was successful */
    if (!bmp_header) {

        printf("Error in check_bmp!\n");
        return NULL;

    }


    /* Open the file */
    file = fopen(path, "rb");


    /* Check if the file was opened */
    if (!file ) {

        printf("Error in check_bmp!\n");
        free(bmp_header);
        return NULL;

    }
    


    /* Read the header */
    if(fread(bmp_header, 1, BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE) {
        printf("Error in check_bmp!\n");
        free(bmp_header);
        fclose(file);
        return NULL;
    }


    /* Check if the file is BMP and if it is 24-bit BMP */
    if ( !( (bmp_header->id_field_1 == BMP_TYPE[0]) && (bmp_header->id_field_2 == BMP_TYPE[1]) ) || (bmp_header->bits_per_pixel != BIT_PER_PIXEL)) {
       
        printf("Invalid BMP sub-format!\nPlease use a %d-bit BMP file.\n", BIT_PER_PIXEL);
        free(bmp_header);
        fclose(file);
        return NULL;
    
    }

    /* Close the file */
    fclose(file);

    bmp_header->path = path;

    /* Return the pointer to the structure */
    return bmp_header;

}

/**
 * This function hide / extract the payload in / from the BMP file.
 * 
 * @param paths Array of paths
 * @param sw Switch
 * 
 * @return 0 if success, 2 not in correct format, 3 if picture is not big enough, 4 no hidden content, 5 damaged content ,6 different error
*/
int proceed_bmp(char **paths, char sw){

    /* Declaration of variables */
	BMP_HEAD *bmp_header;
	png_bytep *row_pointers = NULL;

    /* Sanity check */
    if (!paths) {
        printf("Error in proceed_bmp!\n");
        return FAILURE;
    }

	/* Init pointers */

	int ret = 0;

	/* Read bmp file */

	bmp_header = check_bmp(paths[0]);

	/* If bmp_data is NULL, then the file is not bmp or not in correct format */
	if (!bmp_header) {
    
		return 2;
	}

	ret = read_bmp(bmp_header, &row_pointers);
    
    if (ret == FAILURE) {

        free_bmp_header(bmp_header);
        return ret;
    }


	/* Hide the payload in file*/
	if (sw == 'h') {

		ret = hide_in_image(bmp_header->width, bmp_header->height, &row_pointers, paths[1]);

        if (ret == 3 || ret == 6) {
            
            free_row_pointers(row_pointers, bmp_header->height);
            free_bmp_header(bmp_header);
            return ret;
        }

		ret = write_bmp(&row_pointers, bmp_header);

        if (ret == FAILURE) {

            free_row_pointers(row_pointers, bmp_header->height);
            free_bmp_header(bmp_header);
            return ret;
        
        }


	} else if (sw == 'x') {

		ret = extract_from_image(bmp_header->width, &row_pointers, paths[1]);


	} else {

		printf("Wrong switch\n");
		free_row_pointers(row_pointers, bmp_header->height);
		free_bmp_header(bmp_header);
		ret = 1;
	
	}

    /* Free the memory */

	free_row_pointers(row_pointers, bmp_header->height);
	free_bmp_header(bmp_header);
	return ret;

}