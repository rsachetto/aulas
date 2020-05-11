#include "../plugin_config.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

INIT_PLUGIN(init_plugin) {

	printf("BW PLUGIN INIT\n");
	struct plugin_info info;

	info.params = NULL;

	struct plugin_param_descr x;
	x.name = strdup("x");
	x.description = strdup("Multiply pixels times x");
	x.type = INTEGER;
	arrput(info.params, x);
    
	info.function_name = strdup("apply_bw");
	info.name = strdup("Black and White");
	info.description = strdup("Apply a black and white filter on the image.");
	return info;

}

APPLY_PLUGIN(apply_bw) {
	printf("Calling apply_plugin: Img w: %d, Img h: %d\n", img->width, img->height);

	int img_size = img->width*img->height*3;
	uint8_t *img_data = (uint8_t*)img->data;

	char *x_value = shget(params, "x");
	int x = 1;

	if(x_value != NULL) {
		x = strtol(x_value, NULL, 10);	
	}

	printf("Param x value %d\n", x);

	for(int i = 0; i < img_size; i+=3) {
		uint8_t r = img_data[i];
		uint8_t g = img_data[i + 1];
		uint8_t b = img_data[i + 2];

		uint8_t avg = x*((r+g+b)/3);

		img_data[i] = avg;
		img_data[i + 1] = avg;
		img_data[i + 2] = avg;
	}
}

END_PLUGIN(end_plugin) {
	printf("BW PLUGIN END\n");
}
