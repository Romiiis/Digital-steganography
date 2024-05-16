#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "input.h"


/**
 * This function checks if the filed exists and return paths to the files.
 * 
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @param sw Switch
 * 
 * @return Array of paths or NULL if error
*/
char **get_files(int argc, char *argv[], char *sw){


    /* Declaration and initialize variables */
    int i, step = 0;
    char **paths = NULL;
    FILE *fp = NULL;

    /* Sanity check */
    if (!argv || !sw) {

        printf("Error in get_files!\n");
        return NULL;

    }

    /* Check if the number of arguments is valid */
    if (argc != 4) {
		printf("Invalid usage!\nUse: %s <picture[.bmp]|[.png]> -<h|x> <payload>\n", argv[0]);
		return NULL;
	}


    /* Allocate memory for paths */
    paths = (char **)malloc(sizeof(char *) * NUMBER_OF_PATHS);

    /* Check if the memory was allocated */
    if (!paths) {
        printf("Error in get_files!\n");
        return NULL;
    }


    /* Find the switch */
    for (i = 1; i < argc; i++) {

        /* Check if the argument is a switch */
        if (argv[i][0] == '-') {
            
            /* Check if the switch is valid */
            if (argv[i][1] == 'h' || argv[i][1] == 'x') {

                /* Save the switch */
                *sw = argv[i][1];


            } else {
                    
                printf("Invalid switch!\nUse: %s <picture[.bmp]|[.png]> -<h|x> <payload>\n", argv[0]);

                for (i = 0; i < step; i++) {
                    free(paths[i]);
                }
                
                free(paths);
                return NULL;

            }
            
            break;
        }
    }






    /* Find the paths */
    for (i = 1; i < argc; i++) {

        /* Skip the switch */
        if (argv[i][0] == '-') continue;

        /* If you want to extract, skip the second path (payload) (program will create it) */
        if (*sw != 'x' || step != 1) {

            /* Open the file */
            fp = fopen(argv[i], "rb");

            /* If the file does not exist, print an error and return NULL - free everything */
            if (!fp) {

                printf("Invalid path: %s\n", argv[i]);

                for (i = 0; i < step; i++) {
                    free(paths[i]);
                }
                free(paths);

                return NULL;
            }

        }

        /* Save the path */
        paths[step] = (char *)malloc(sizeof(char) * (strlen(argv[i]) + 1));

        /* Check if the memory was allocated */
        if (!paths[step]) {

            printf("Error in get_files!\n");

            for (i = 0; i < step; i++) {
                free(paths[i]);
            }

            free(paths);

            return NULL;

        }
        
        /* Copy the path */
        strcpy(paths[step], argv[i]);

        /* Increment the step */
        step++;

        /* Close the file */
        if (*sw != 'x' || step != 1) {

            fclose(fp);

        }

    }


    /* Return paths */
    return paths;

}



/**
 * This function checks if the file is bmp or png.
 * 
 * @param path Path to the file
 * 
 * @return 0 if bmp, 1 if png, -1 if error
*/
int check_picture(char *path){
    
    /* Declaration variables */
    char *suffix;

    /* Sanity check */
    if (!path) {

        printf("Error in check_picture!\n");
        return FAILURE;

    }

    /* Find last dot */
    suffix = strrchr(path, '.');

    /* Check if the suffix is valid  and return 0 if bmp, 1 if png, -1 if error */
    if (strcmp(suffix, suffix1) == 0) return BMP;    
    else if (strcmp(suffix, suffix2) == 0) return PNG;
    else {

        printf("Invalid image format!\n Use .bmp or .png (24-bit/px RGB)\n");
        return FAILURE;
        
    }

}


/**
 * This function frees the payload.
 * 
 * @param payload Payload to be freed
 * 
 * @return void
*/
void free_payload(payload *payload){

    /* Sanity check */
    if (!payload) {
        printf("Error in free_payload!\n");
        return;
    }

    /* Free the data */
    if (payload->data) {

        free(payload->data);
    }


    free(payload);
    
}


/**
 * This function returns the payload from the file.
 * 
 * @param path Path to the payload
 * 
 * @return Payload or NULL if error
*/
payload *get_payload(char *path){
    
    /* Declaration and initialization variables */
    FILE *file = NULL;
    int i;
    payload *pt = NULL;
    
    /* Sanity check */
    if (!path) {
        printf("Error in get_payload!\n");
        return NULL;
    }

    /* Allocate memory for payload */
    pt = (payload *)malloc(sizeof(payload));

    /* Check if the memory was allocated */
    if (!pt) {
        printf("Error in get_payload!\n");
        return NULL;
    }
    
    /* Open the file */
    file = fopen(path, "rb");

    /* Check if the file exists */
    if (!file) {

        free(pt);
        printf("Invalid path: %s\n", path);
        return NULL;
    }

    /* Get in the end of the file */
    if ( (fseek(file, 0, SEEK_END)) != 0 ) {
            
        free(pt);
        printf("Error in get_payload!\n");
        return NULL;
    }

    /* Save the size */
    pt->size = ftell(file);

    /* Get in the beginning of the file */
    if ( (fseek(file, 0, SEEK_SET)) != 0) {
            
        free(pt);
        printf("Error in get_payload!\n");
        return NULL;

    }

    /* Allocate memory for data */
    pt->data = (byte *)malloc(pt->size + 1);

    /* Check if the memory was allocated */
    if (!(pt->data)) {

        free(pt);
        printf("Error in get_payload!\n");
        return NULL;

    }

    /* Read the data */
    for (i = 0; i < pt->size; i++) {
        pt->data[i] = fgetc(file);
    }

    /* Add the null terminator */
    pt->data[pt->size] = '\0';

    /* Close the file */
    fclose(file);

    /* Return the payload */
    return pt;



}

