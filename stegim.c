#include <stdio.h>
#include <stdlib.h>
#include "modules/lzw.h"
#include "modules/input.h"
#include "modules/bmp_lib.h"
#include "modules/png_lib.h"
#include "modules/pixel_secrets.h"
#include "modules/my_defs.h"


/**
 * This function is entry point of the program.
 * 
 * @param argc Count of arguments
 * @param argv Array of arguments
 * @return 0 if success, 
 */
int main(int argc, char *argv[]){

	/* Declaration of variables */
	char sw, **paths;
	int exit_code = 0;

	/* Check if the number of arguments is valid and paths are valid */
	paths = get_files(argc, argv, &sw);

	/* Check if everything is ok */
	if (!paths) {

		/* WRONG PARAMETERS 1*/
		return 1;
	}


	/* Check if the picture is bmp or png */
	exit_code = check_picture(paths[0]);

	switch (exit_code) {
		case BMP: {
			/* BMP */
			exit_code = proceed_bmp(paths, sw);
			break;
		}
		case PNG: {
			
			/* PNG */
			exit_code = proceed_png(paths, sw);
			break;
		}
		case FAILURE: {
			
			/* ERROR */
			exit_code = 2;
			break;
		}
	}
	
	/* Free memory */
	free(paths[0]);
	free(paths[1]);
	free(paths);

	return exit_code;


}







