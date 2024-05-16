/*  INPUT.H */

#ifndef __input_h__
#define __input_h__

#include "my_defs.h"

/* ----------Defines---------- */


#define suffix1 ".bmp"
#define suffix2 ".png"
#define NUMBER_OF_PATHS 2

#define BMP 0
#define PNG 1




/* ----------Structures---------- */

typedef struct{
    long size;
    byte *data;
} payload;



/* ----------Prototypes---------- */


/**
 * This function checks if the filed exists and return paths to the files.
 * 
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @param sw Switch
 * 
 * @return Array of paths or NULL if error
*/
char **get_files(int argc, char *argv[], char *sw);


/**
 * This function returns the payload from the file.
 * 
 * @param path Path to the payload
 * 
 * @return Payload or NULL if error
*/
payload *get_payload(char *path);


/**
 * This function frees the payload.
 * 
 * @param payload Payload to be freed
 * 
 * @return void
*/
void free_payload(payload *payload);


/**
 * This function checks if the file is bmp or png.
 * 
 * @param path Path to the file
 * 
 * @return 0 if bmp, 1 if png, FAILURE if error
*/
int check_picture(char *path);




#endif