#include "../plugin_config.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <assert.h>

#define MAX_BRIGHTNESS 255.0

#ifndef M_PI
// C99 doesn't define M_PI (GNU-C99 does)
#define M_PI 3.14159265358979323846264338327
#endif

inline static uint8_t GET_PIXEL_CHECK(struct rgb_pixel *img, int nx, int ny, int x, int y, int l) {
  if ( (x<0) || (x >= nx) || (y<0) || (y >= ny) ) return 0;

  switch(l) {
	case 0: return img[(x * ny + y)].r;
	case 1: return img[(x * ny + y)].g;
	case 2: return img[(x * ny + y)].b;
	default: return 0;
  }

}

void convolution(struct rgb_pixel *im, float *K, int nx, int ny, int kn, bool normalize, float divisor, float offset) {

	unsigned int ix, iy, l;
	int kx, ky;
	float cp[3];
	assert(kn % 2 == 1);
	assert(nx > kn && ny > kn);

	const int Ks = kn / 2;

	struct rgb_pixel *copy = (struct rgb_pixel*)malloc(sizeof(struct rgb_pixel)*nx*ny);
	memcpy(copy, im, sizeof(struct rgb_pixel)*nx*ny);

	for(ix=0; ix < nx; ix++) {
		for(iy=0; iy < ny; iy++) {
			cp[0] = cp[1] = cp[2] = 0.0;
			for(kx=-Ks; kx <= Ks; kx++) {
				for(ky=-Ks; ky <= Ks; ky++) {
					for(l=0; l<3; l++) {
						float kernel_value = (K[(kx+Ks)+(ky+Ks)*(2*Ks+1)]/divisor);
						cp[l] += kernel_value*((float)GET_PIXEL_CHECK(copy, nx, ny, ix+kx, iy+ky, l)) + offset;
					}
				}
			}


			//TODO:
			//normalize: Se esta opção estiver marcada, o Divisor pegará o valor resultante da convolução. 
			//Se o resultado for zero (não é possível dividir por zero), então um valor de deslocamento de 128 será aplicado. 
			//Se ele for negativo (um valor de cor negativo não existe), então um valor de deslocamento de 255 será aplicado (o resultado será invertido).
			//https://docs.gimp.org/2.8/pt_BR/plug-in-convmatrix.html

			for(l=0; l<3; l++)
				cp[l] = (cp[l]>255.0) ? 255.0 : ((cp[l]<0.0) ? 0.0 : cp[l]);


			im[(ix * ny + iy)].r = cp[0];
			im[(ix * ny + iy)].g = cp[1];
			im[(ix * ny + iy)].b = cp[2];

		}
	}
}


/*
 * gaussianFilter:
 * http://www.songho.ca/dsp/cannyedge/cannyedge.html
 * determine size of kernel (odd #)
 * 0.0 <= sigma < 0.5 : 3
 * 0.5 <= sigma < 1.0 : 5
 * 1.0 <= sigma < 1.5 : 7
 * 1.5 <= sigma < 2.0 : 9
 * 2.0 <= sigma < 2.5 : 11
 * 2.5 <= sigma < 3.0 : 13 ...
 * kernelSize = 2 * int(2*sigma) + 3;
 */
void gaussian_filter_func(uint8_t *out, const int nx, const int ny, const float sigma) {
	const int n = 2 * (int)(2 * sigma) + 3;
	const float mean = (float)floor(n / 2.0);
	float kernel[n * n]; // variable length array

	fprintf(stderr, "gaussian_filter: kernel size %d, sigma=%g\n", n, sigma);

	size_t c = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) {
			kernel[c] = exp(-0.5 * (pow((i - mean) / sigma, 2.0) + pow((j - mean) / sigma, 2.0)))/(2 * M_PI * sigma * sigma);
			c++;
		}

	convolution((struct rgb_pixel*)out, kernel, nx, ny, n, false, 1, 0);
}

void edge_detection_filter(uint8_t *out, const int nx, const int ny) {

	int n = 3;
	float kernel[n * n]; // variable length array

	/*
	size_t c = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			kernel[c] = -1;
			c++;
		}
	}

	kernel[4] = 8;
	*/

	//Edge detection kernel
	kernel[0] = 0;
	kernel[1] = 1;
	kernel[2] = 0;
	kernel[3] = 1;
	kernel[4] = -4;
	kernel[5] = 1;
	kernel[6] = 0;
	kernel[7] = 1;
	kernel[8] = 0;


//	size_t c = 0;
//	for (int i = 0; i < n; i++) {
//		for (int j = 0; j < n; j++) {
//			kernel[c] = 0;
//			c++;
//		}
//	}

	////Aguçar
	//kernel[7]  = -1;
///	kernel[11] = -1;
//	kernel[12] =  5;
//	kernel[13] = -1;
//	kernel[17] = -1;

	//desfocar
//	kernel[6]  = 1;
//	kernel[7]  = 1;
//	kernel[8]  = 1;
//	kernel[11] = 1;
//	kernel[12] = 1;
//	kernel[13] = 1;
//	kernel[16] = 1;
//	kernel[17] = 1;
//	kernel[18] = 1;


	convolution((struct rgb_pixel*)out, kernel, nx, ny, n, false, 1, 0);
}

INIT_PLUGIN(init_plugin) {

	START_PLUGIN_INFO;

	SET_PLUGIN_NAME("Convolution");
	SET_PLUGIN_DESCRIPTION("Colection of plugins that can be used to perform convolution with predermined kernels");

	ADD_PLUGIN_FUNCTION("gaussian_filter");

	END_PLUGIN_INFO;

}

APPLY_PLUGIN(gaussian_filter) {
//	gaussian_filter_func(img->data, img->height, img->width, 0.5);
	edge_detection_filter(img->data, img->height, img->width);
}

END_PLUGIN(end_plugin) {
}
