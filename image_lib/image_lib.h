#ifndef IMAGE_LIB_H
#define IMAGE_LIB_H

#include <stdint.h>
#include <stdbool.h>

#define STR_EQUAL_I(str1, str2) (strcasecmp(str1, str2) == 0)
#define IS_NET_PBM(extension) ( STR_EQUAL_I(extension, "PPM") || STR_EQUAL_I(extension, "PGM") || STR_EQUAL_I(extension, "PBM") )
#define IS_BMP(extension) ( STR_EQUAL_I(extension, "bmp") )
#define IS_SIF(extension) ( STR_EQUAL_I(extension, "sif") )
#define IS_WEBP(extension) ( STR_EQUAL_I(extension, "webp") )

#define CHECK_PATH(img_path, ret)                                      \
do {                                                                   \
	if(!(img_path)) {                                                  \
		fprintf(stderr, "NULL image path in function %s\n", __func__); \
		return (ret);                                                  \
	}																   \
}		                                                               \
while(0)

#define CHECK_IMAGE_FILE(img_path, f_info, ret)                                             \
	do {                                                                                    \
		if(!(f_info.is_file) || !(file_info.exists)) {                                      \
			fprintf(stderr, "Invalid image file %s in function %s\n", (img_path), __func__);\
			return (ret);                                                                   \
		}																                    \
	}		                                                                                \
	while(0)

#define CHECK_FOR_ERRORS(img_path, img)	       \
	CHECK_PATH(img_path, img);                 \
	struct path_information file_info;         \
	get_path_information(img_path, &file_info);\
	CHECK_IMAGE_FILE(img_path, file_info, img);


typedef enum {
	NETPBM_PBM,
	NETPBM_PGM,
	NETPBM_PPM,
	MS_BMP_RGB24
} image_format;

typedef enum {
	UR8G8B8,
	UR8G8B8A8
} pixel_format;

typedef struct {
	uint32_t ciexyzX, ciexyzY, ciexyzZ;
} __attribute__ ((packed)) CIEXYZ;
	
typedef struct {
	CIEXYZ ciexyzRed, ciexyzGreen, ciexyBlue;
}__attribute__ ((packed)) CIEXYZTRIPLE;

typedef struct {		
	uint32_t size;	 //Size of InfoHeader - we have to use this information to load the correct profile

	uint32_t width;	 //Horizontal width of bitmap in pixels
	uint32_t height; //Vertical height of bitmap in pixels

	uint16_t planes; //Number of Planes (=1)

	uint16_t bits_per_pixel;   //Bits per Pixel used to store palette entry information. This also identifies in an indirect way the number of possible colors. Possible values are:
						       //1 = monochrome palette. NumColors = 1  
							   //4 = 4bit palletized. NumColors = 16  
							   //8 = 8bit palletized. NumColors = 256 
						       //16 = 16bit RGB. NumColors = 65536
							   //24 = 24bit RGB. NumColors = 16M

	uint32_t compression;	   //Type of Compression  
						       //0 = BI_RGB   no compression  
							   //1 = BI_RLE8 8bit RLE encoding  
							   //2 = BI_RLE4 4bit RLE encoding

	uint32_t image_size;	       // (compressed) Size of Image 
							   //It is valid to set this =0 if Compression = 0
	
	uint32_t x_pixels_per_m;   //horizontal resolution: Pixels/meter
	uint32_t y_pixels_per_m;   //vertical resolution: Pixels/meter
	
	uint32_t colors_used;      //Number of actually used colors. For a 8-bit / pixel bitmap this will be 100h or 256.

	uint32_t important_colors; //Number of important colors 
							   //0 = all

	uint32_t red_mask;
	uint32_t green_mask;
	uint32_t blue_mask;
	uint32_t alpha_mask;
	uint32_t cs_type;
	
	CIEXYZTRIPLE endpoints;
    uint32_t gamma_red;
    uint32_t gamma_green;
    uint32_t gamma_blue;
	uint32_t intent;
	uint32_t profile_data;
	uint32_t profile_size;
	uint32_t reserved;
							   
} __attribute__ ((packed)) bmp_info_header;

typedef struct {		
	uint16_t signature; //"BM" 
	uint32_t file_size; //File size in bytes
	uint32_t reserved; //unused (=0)
	uint32_t data_offset; //Offset from beginning of file to the beginning of the bitmap data
} __attribute__ ((packed)) bmp_header;

typedef struct {
	char riff[4];          //String "RIFF"
	uint32_t block_length; //A little-endian 32 bit value of the block length, the whole size of the block controlled by the RIFF header. 
						   //Normally this equals the payload size (file size minus 8 bytes: 4 bytes for the 'RIFF' identifier and 4 bytes for storing the value itself).

    char webp[4];	       //String "WEBP" (RIFF container name).
	char vp8[4];           //String "VP8L" (chunk tag for lossless encoded image data).
	uint32_t num_bytes;    //A little-endian 32-bit value of the number of bytes in the lossless stream.
	uint8_t signature;      //One byte signature 0x2f.

} __attribute__((packed)) webp_header;

typedef struct {
	char start[3];          //String "SIF"
	uint32_t width;	        //Horizontal width of the image in pixels
	uint32_t height;        //Vertical height of image in pixels
	uint16_t bits_per_pixel;   //Bits per Pixel used to store palette entry information. This also identifies in an indirect way the number of possible colors. Possible values are:
						       //1 = monochrome palette. NumColors = 1  
							   //4 = 4bit palletized. NumColors = 16  
							   //8 = 8bit palletized. NumColors = 256 
						       //16 = 16bit RGB. NumColors = 65536
							   //24 = 24bit RGB. NumColors = 16M

	uint8_t compressed;	   //1 if the pixels are compressed; 0 otherwise 
	uint8_t has_alpha;	   //1 if the alpha channel is present; 0 otherwise 

} __attribute__((packed)) sif_header;

typedef struct image_t {
	uint32_t width;
	uint32_t height;
	void *data;
	image_format format;
	pixel_format pixel_format;

} image;

image load_image(char *image_path);
//image load_webp(char *image_path);
image load_netpbm(char *image_path);
image load_bmp(char *image_path);
image load_sif(char *image_path);
int write_image_as_sif(image img, char *save_path, bool compress);
void free_image(image img);

#endif
