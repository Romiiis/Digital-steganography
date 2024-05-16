/* PNG_LIB.C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <png.h>
#include "png_lib.h"
#include "pixel_secrets.h"



/**
 * This function read the PNG file and store the data in the row_pointers.
 * 
 * @param filename Name of the file
 * @param width Pointer to the width
 * @param height Pointer to the height
 * @param row_pointers Pointer to the array of png_bytep
 * @param png Pointer to the png_structp
 * @param info Pointer to the png_infop
 * 
 * @return SUCCESS or FAILURE
*/
int read_png(char *filename, int *width, int *height, png_bytep **row_pointers, png_structp *png, png_infop *info) {
    
    /* Declaration of variables */
    FILE *fp;
    int y;

    /* Sanity check */
    if (!filename || !width || !height || !row_pointers || !png || !info) {
        printf("Error in read_png!\n");
        return FAILURE;
    }

    /* Open the file */
    fp = fopen(filename, "rb");

    /* Check if the file was opened */
    if (!fp) {
        printf("Error in read_png!\n");
        return FAILURE;
    }

    /* Check if the file can be read as PNG */
    *png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    /* Check if it was created */
    if (!(*png)) {
        printf("Error in read_png!\n");
        fclose(fp);
        return FAILURE;
    }

    /* Check if the info struct was created */
    *info = png_create_info_struct(*png);

    /* Check if it was created */
    if (!(*info)) {

        printf("Error in read_png!\n");
        png_destroy_read_struct(png, info, NULL);
        fclose(fp);
        return FAILURE;
    }

    /* Check if the setjmp was set */
    if (setjmp(png_jmpbuf(*png))) {

        printf("Error in read_png!\n");
        png_destroy_read_struct(png, info, NULL);
        fclose(fp);
        return FAILURE;
    }

    /* Initialize the IO */
    png_init_io(*png, fp);

    /* Read the info */
    png_read_info(*png, *info);

    /* Get the width and height */
    *width = png_get_image_width(*png, *info);
    *height = png_get_image_height(*png, *info);

    /* Get the color type */
    png_read_update_info(*png, *info);

    /* Allocate memory for row_pointers */
    *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * (*height));

    /* Check if the memory was allocated */
    if (!(*row_pointers)) {
        printf("Error in read_png!\n");
        png_destroy_read_struct(png, info, NULL);
        fclose(fp);
        return FAILURE;
    }

    /* Allocate memory for each row */
    for (y = 0; y < *height; y++) {

        (*row_pointers)[y] = (png_byte *)malloc(png_get_rowbytes(*png, *info));

        /* Check if the memory was allocated */
        if (!(*row_pointers)[y]) {
            printf("Error in read_png!\n");

            /* Free the allocated memory */
            for (int i = 0; i < y; i++) {
                free((*row_pointers)[i]);
            }

            free(*row_pointers);


            png_destroy_read_struct(png, info, NULL);
            fclose(fp);
            return FAILURE;
        }

    }
    /* Read the image */
    png_read_image(*png, *row_pointers);

    /* Close the file */
    fclose(fp);

    return SUCCESS;
}



/**
 * This function write the PNG file from the row_pointers.
 * 
 * @param filename Name of the file
 * @param width Width of the image
 * @param height Height of the image
 * @param row_pointers Pointer to the array of png_bytep
 * @param read_png Pointer to the png_structp
 * @param read_info Pointer to the png_infop
 * 
 * @return SUCCESS or FAILURE
*/
int write_png_file(char *filename, png_bytep *row_pointers, png_structp read_png, png_infop read_info) {

    /* Declaration of variables */
    FILE *fp;
    png_uint_32 width_orig, height_orig;
    int bit_depth_orig, color_type_orig;

    /* Sanity check */
    if (!filename || !row_pointers || !read_png || !read_info) {
        printf("Error in write_png_file!\n");
        return FAILURE;
    }

    /* Open the file */
    fp = fopen(filename, "wb");

    /* Check if the file was opened */
    if (!fp) {
        printf("Error in write_png_file!\n");
        return FAILURE;
    }

    /* Check if the file can be read as PNG */
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);


    if (!png) {
        printf("Error in write_png_file!\n");
        fclose(fp);
        return FAILURE;
    }

    /* Check if the info struct was created */
    png_infop info = png_create_info_struct(png);

    /* Check if it was created */
    if (!info) {
        printf("Error in write_png_file!\n");
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return FAILURE;
    }

    /* Check if the setjmp was set */
    if (setjmp(png_jmpbuf(png))) {
        printf("Error in write_png_file!\n");
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return FAILURE;
    }

    /* Initialize the IO */
    png_init_io(png, fp);

    /* Get the image attributes */
    png_get_IHDR(read_png, read_info, &width_orig, &height_orig, &bit_depth_orig, &color_type_orig, NULL, NULL, NULL);

    /* Set the image attributes */
    png_set_IHDR(
        png,
        info,
        width_orig, height_orig,
        bit_depth_orig,
        color_type_orig,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    /* Write the info */
    png_write_info(png, info);

    /* Write the image */
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);


    /* Close the file */
    fclose(fp);

    /* Free the memory */
    png_destroy_write_struct(&png, &info);

    return 0;
}

/**
 * This function checks if file is PNG file in correct format
 * 
 * @param path Path to the file
 * 
 * @return Count of pixels if png, FAILURE if error
*/
int check_png(char *path){

    /* Declaration and initialization of variables */
    FILE *fp;
    int bit_depth, color_type;

    /* Sanity check */
    if (!path) {
        printf("Error in check_png!\n");
        return FAILURE;
    }


    /* Open the file */
    fp = fopen(path, "rb");

    if (!fp) {
        printf("Error in check_png!\n");
        return FAILURE;
    }

    /* Check if the file can be read as PNG */
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    /* Check if it was created */
    if (!png) {

        printf("Error in check_png!\n");
        fclose(fp);
        return FAILURE;
    }
        
    /* Check if the info struct was created */
    png_infop info = png_create_info_struct(png);

    /* Check if it was created */
    if (!info) {

        printf("Error in check_png!\n");
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        return FAILURE;

    }


    if (setjmp(png_jmpbuf(png))) {

        printf("Error in check_png!\n");
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return FAILURE;
    }

    /* Initialize the IO */
    png_init_io(png, fp);

    /* Read the info */
    png_read_info(png, info);

    /* Get the bit depth and color type */
    bit_depth = png_get_bit_depth(png, info);
    color_type = png_get_color_type(png, info);

    /* Check if the file is in correct format */
    if (bit_depth != 8 || color_type != PNG_COLOR_TYPE_RGB) {

        /* Free the memory */
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        
        printf("Invalid PNG subformat!\nPlease use a 24-bit PNG file.\n");
        return FAILURE;

    } 

   
    /* Close */
    fclose(fp);

    /* Free the memory */
    png_destroy_read_struct(&png, &info, NULL);


    /* Return count of pixels in image */
    return (png_get_image_width(png, info) * png_get_image_height(png, info));

}


/**
 * This function will hide / extract the payload in / from the PNG file.
 * 
 * @param paths Array of paths
 * @param sw Switch
 * 
 * @return 0 if success, 2 not in correct format, 3 if picture is not big enough, 4 no hidden content, 5 damagged content ,6 different errror
*/
int proceed_png(char **paths, char sw){

    /* Declaration of variables */
	int result = 0, width, height, exit_code = 0;
	png_bytep *row_pointers = NULL;
	png_structp png;
	png_infop info;

    /* Sanity check */
    if (!paths) {
        printf("Error in proceed_png!\n");
        return FAILURE;
    }

    /* Check if the png is in correct format */
	result = check_png(paths[0]);


	/* -1 indicate that the png is not 24 bit*/
	if (result == FAILURE) {
		return 2;
	}


	/* Read the png file */
	exit_code = read_png(paths[0], &width, &height, &row_pointers, &png, &info);

    /* Check if the file was read */
    if (exit_code == FAILURE) {
        printf("Error in proceed_png!\n");
        return FAILURE;
    }
	

	if (sw == 'h') {

		exit_code = hide_in_image(width, height, &row_pointers, paths[1]);

        if(exit_code == 3){

            /* Free memory */
            free_row_pointers(row_pointers, height);
            png_destroy_read_struct(&png, &info, NULL);

            return 3;
        }


		exit_code = write_png_file(paths[0], row_pointers, png, info);

        if (exit_code == FAILURE) {
            
            /* Free memory */
            free_row_pointers(row_pointers, height);
            png_destroy_read_struct(&png, &info, NULL);
            return 6;
        }

	} else if (sw == 'x') {

		exit_code = extract_from_image(width, &row_pointers, paths[1]);

        

	}


	/* Free memory */
	free_row_pointers(row_pointers, height);
	png_destroy_read_struct(&png, &info, NULL);
	

	return exit_code;
}







