#include "image_lib/image_lib.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char**argv) {

	char *input = argv[1];
	char *output = argv[2];

	image img = load_bmp(input);
	write_image_as_sif(img, output, true);

}
