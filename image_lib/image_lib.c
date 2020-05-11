#include "image_lib.h"
#include "utils/file_utils.h"
#include "utils/data_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

static inline uint8_t convert_color_space(int max_color, int color) {
    return (uint8_t) ( ( (float)color / max_color) * 255.0 );
}

image load_image(char *image_path) {

	image img = { 0 };

	struct path_information file_info;

	get_path_information(image_path, &file_info);
	
	if(IS_NET_PBM(file_info.file_extension)) {
		img = load_netpbm(image_path);
		img.pixel_format = UR8G8B8;
	}
	else if(IS_BMP(file_info.file_extension)) {
		img = load_bmp(image_path);
		img.pixel_format = UR8G8B8;
	}
    else if(IS_SIF(file_info.file_extension)) {
          img = load_sif(image_path);
    }
	//else if(IS_WEBP(file_info.file_extension)) {
	//	img = load_webp(image_path);
	//}

	return img;
}

void free_image(image img) {
	free(img.data);
}

// Treating buf[] as a giant little-endian integer, grab "width"
// bits starting at bit number "pos" (LSB=bit 0).
uint64_t bit_extract_lsb(const uint8_t *buf, size_t pos, int width) {
    assert(width >= 0 && width <= 64 - 7);

    // Read a 64-bit little-endian number starting from the byte
    // containing bit number "pos" (relative to "buf").
    uint64_t bits = *((uint64_t*)(&buf[pos / 8]));

    // Shift out the bits inside the first byte that we've
    // already consumed.
    // After this, the LSB of our bit field is in the LSB of bits.
    bits >>= pos % 8;

    // Return the low "width" bits, zeroing the rest via bit mask.
    return bits & ((1ull << width) - 1);
}

uint64_t read_bits(uint8_t *bitstream, int width) {

	static size_t bit_pos = 0;
    // Read the bits
    uint64_t result = bit_extract_lsb(bitstream, bit_pos, width);
    // Advance the cursor
    bit_pos += width;
    return result;
}

enum TransformType {
  PREDICTOR_TRANSFORM             = 0,
  COLOR_TRANSFORM                 = 1,
  SUBTRACT_GREEN                  = 2,
  COLOR_INDEXING_TRANSFORM        = 3,
};

#define DIV_ROUND_UP(num, den) (((num) + (den) - 1) / (den))

struct predictor_transform_data {
	int size_bits;
	int block_width;
	int block_height;
	int block_xsize; 
};

image load_webp(char *image_path) {
	
	image img = { 0 };

	size_t file_size;
	char *image_data = read_entire_file_with_mmap(image_path, &file_size);

	uint8_t *data = (uint8_t*)image_data;
	webp_header header = *((webp_header*) data);
	data += sizeof(header);
	assert(header.signature == 0x2f);
	
	int image_width = read_bits(data, 14) + 1;
	int image_height = read_bits(data, 14) + 1;
	int alpha_is_used = read_bits(data, 1);
	int version_number = read_bits(data, 3);

	printf("image_width: %d, image_width: %d\n", image_width, image_height);
	printf("alpha_is_used: %d\n", alpha_is_used);
	printf("version: %d\n", version_number);

	bool subtract_green = false;
	bool predictor_transform = false;

	struct predictor_transform_data pt_data;

	assert(version_number == 0);

	int color_cache_used;
	int huffman_code_length;

	while (read_bits(data, 1)) {  // Transform present.
    	// Decode transform type.
		enum TransformType transform_type = read_bits(data, 2);
		printf("TransformType: %d\n", transform_type);

		switch(transform_type) {
			case PREDICTOR_TRANSFORM:
				predictor_transform = true;
				pt_data.size_bits = read_bits(data, 3) + 2;
				pt_data.block_width = (1 << pt_data.size_bits);
				pt_data.block_height = (1 << pt_data.size_bits);
				pt_data.block_xsize = DIV_ROUND_UP(image_width, 1 << pt_data.size_bits);

				color_cache_used = read_bits(data, 1);
				printf("color_cache_used: %d\n", color_cache_used);

				huffman_code_length = read_bits(data, 1);
				printf("huffman_code_length: %d\n", huffman_code_length);

				if(huffman_code_length == 0) {
					int kCodeLengthCodeOrder[19] = {17, 18, 0, 1, 2, 3, 4, 5, 16, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
					int code_lengths[19] = { 0 };  // All zeros.
					int num_code_lengths = 4 + read_bits(data, 4);
					printf("num_code_lengths: %d\n", num_code_lengths);

					for (int i = 0; i < num_code_lengths; ++i) {
						code_lengths[kCodeLengthCodeOrder[i]] = read_bits(data, 3);
						printf("%d\n", 	code_lengths[kCodeLengthCodeOrder[i]] );
					}
				}

				break;
			case SUBTRACT_GREEN:
				subtract_green = true;
				break;//no data associated to this transfor
		}
	   // Decode transform data.

	}

	return img;

}

image load_bmp(char *image_path) {

	image img = { 0 };

	size_t file_size;
	char *image_data = read_entire_file_with_mmap(image_path, &file_size);

	char *src_string = image_data;
	bmp_header header = *((bmp_header*) src_string);
	src_string += sizeof(header);

#ifdef DEBUG_INFO
	char *str = (char *) &header.signature;
	printf("BMP header:\n");
	printf("  Signature: %.*s\n", 2, str);
	printf("  File size: %d\n", header.file_size);
	printf("  Data offset: %d\n", header.data_offset);
#endif

	bmp_info_header info_header = *((bmp_info_header*) src_string);
	src_string += info_header.size;

#ifdef DEBUG_INFO
	printf("BMP info_header:\n");
	printf("  Header Size: %d\n", info_header.size);
	printf("  Image width: %d\n", info_header.width);
	printf("  Image height: %d\n", info_header.height);
	printf("  Image bits per pixel: %d\n", info_header.bits_per_pixel);
	printf("  Image compression: %d\n", info_header.compression);
	printf("  Image raw size: %d\n", info_header.image_size);
	printf("  Image colors used: %d\n", info_header.colors_used);
#endif

	if(info_header.bits_per_pixel == 24 || !info_header.compression) {
		
		img.format = MS_BMP_RGB24;

		uint32_t w = img.width  = info_header.width;
		uint32_t h = img.height = info_header.height;
	
		img.data = malloc(w*h*3);

		uint8_t *byte_data = (uint8_t*)img.data;

		src_string = image_data + header.data_offset;

		uint32_t row_size_packed = ceil((24.0*w)/32.0)*4; //in bytes
		uint32_t row_size_real   = 3*w; //3 bytes per pixel
		uint32_t extra_bytes     = row_size_packed - row_size_real;

		assert(extra_bytes >= 0);

		for(uint32_t i = 0; i < h; i++) {
			for(uint32_t j = 0; j < w; j++) {								
				
				uint64_t pixel_position = ((h - i - 1)*w + j)*3;

				uint8_t b = *src_string;
				src_string++;
				
				uint8_t g = *src_string;
				src_string++;				

				uint8_t r = *src_string;
				src_string++;

				byte_data[pixel_position]   = r;
				byte_data[pixel_position+1] = g;
				byte_data[pixel_position+2] = b;
			}

			src_string += extra_bytes;
		}

	}
	else {
		fprintf(stderr, "Compressed RGB not supported yet. We only support uncompressed RGB24 files!\n");
	}

	return img;

}

static void read_bpm_data(image *img, char *src_string) {

	uint32_t width = img->width;
	uint32_t height = img->height;

	if(!width || !height) {
		fprintf(stderr, "Error on function %s\n", __func__);
		fprintf(stderr, "width=%d, height=%d are invalid!\n", width, height);
		img->data = NULL;
		return;
	}

	uint64_t data_to_load = width*height*3;

	//ALWAYS READING RGBA USING 32BITS
	void *data = malloc(sizeof(uint8_t)*data_to_load);
	uint8_t *typed_data = (uint8_t*)data;

	uint64_t data_count = 0;

	while(data_count < data_to_load) {

		//ignoring withe spaces
		while(*src_string == ' ' || *src_string == '\n') src_string++;

		if(isdigit(*src_string)) {		

			while(isdigit(*src_string)) {
				if(*src_string == '0') {
					typed_data[data_count]     = 255;	
					typed_data[data_count + 1] = 255;	
					typed_data[data_count + 2] = 255;	

					src_string++;
					data_count+=3;

				}
				else if(*src_string == '1') {
					typed_data[data_count]     = 0;	
					typed_data[data_count + 1] = 0;	
					typed_data[data_count + 2] = 0;	

					src_string++;
					data_count+=3;
				}
				else {
					fprintf(stderr, "Invalid character of BPM image file %c\n", *src_string);
					free(data);
					data = NULL;
				}
			}

		}
		else {
			fprintf(stderr, "Invalid character of BPM image file %c\n", *src_string);
			free(data);
			data = NULL;

		}

	}

	img->data = data;
	return;

}

static void read_pgm_or_ppm_data(image *img, char *src_string) {

	uint32_t width = img->width;
	uint32_t height = img->height;

	if(!width || !height) {
		fprintf(stderr, "Error on function %s\n", __func__);
		fprintf(stderr, "width=%d, height=%d are invalid!\n", width, height);
		img->data = NULL;
		return;
	}

	uint64_t data_to_load = width*height*3;
	 
	void *data = malloc(sizeof(uint8_t)*data_to_load);
	uint8_t *typed_data = (uint8_t*)data;
	uint8_t int_data = 0;

	uint64_t data_count = 0;
	bool read_color_range = false;
	uint8_t color_range;

	while(data_count < data_to_load) {

		//ignoring withe spaces
		while(*src_string == ' ' || *src_string == '\n') src_string++;

		if(isdigit(*src_string)) {

			while(isdigit(*src_string)) {
				int_data = int_data*10 + *src_string - '0';
				src_string++;
			}					

			if(!read_color_range) {
				color_range = int_data;
				read_color_range = true;
			}
			else {

				if(img->format == NETPBM_PGM) {
					uint8_t c = convert_color_space(color_range, int_data);
					typed_data[data_count   ]  = c;
					typed_data[data_count + 1] = c;
					typed_data[data_count + 2] = c;
					data_count+=3;
				}
				else if (img->format == NETPBM_PPM) {
					typed_data[data_count] = convert_color_space(color_range, int_data);
					data_count++;
				}
			}

			int_data = 0;
		}
		else {
			fprintf(stderr, "Invalid character of PGM or PPM image file %c\n", *src_string);
			free(data);
			img->data = NULL;
			return;

		}

	}

	img->data = data;
	return;

}

image load_netpbm(char *image_path) {

	image img = {0};
	
	CHECK_FOR_ERRORS(image_path, img);

	size_t file_size;

	char *image_data = read_entire_file_with_mmap(image_path, &file_size);	
	char *src_string = image_data;
	
	char img_type = '0';
	
	int width = 0;
	int height = 0;

	while(*src_string) {

		//ignoring withe spaces
		while(*src_string == ' ' || *src_string == '\n') src_string++;

		//ignore comments
		if(*src_string == '#') {
			while(*src_string != '\n') {
				src_string++;
			}
			continue;
		}

		//we found the header
		if(*src_string == 'P') {
			src_string++;

			//TODO: check if the type is valid...
			img_type = *src_string;

			src_string++;
			continue;
		}

		//TODO: check for errors
		if(isdigit(*src_string)) {

			while(isdigit(*src_string)) {
				width = width*10 + *src_string - '0';
				src_string++;
			}

			while(*src_string == ' ' || *src_string == '\n') src_string++;

			while(isdigit(*src_string)) {
				height = height*10 + *src_string - '0';
				src_string++;
			}

			//we read the header. Now we can start reading the data
			break;

		}	

	}

	printf("W: %d, H: %d, type: P%c\n", width, height, img_type);

	img.width = width;
	img.height = height;

	switch(img_type) {
		case '1':
			read_bpm_data(&img, src_string);
			img.format = NETPBM_PBM;
			break;
		case '2':
			img.format = NETPBM_PGM;
			read_pgm_or_ppm_data(&img, src_string);
			break;
		case '3':
			img.format = NETPBM_PPM;
			read_pgm_or_ppm_data(&img, src_string);
			break;

		default: 
			fprintf(stderr, "NETPBM P%c type not supported yet\n", img_type);
			return img;
	}

	munmap(image_data, file_size);
	return img;

}

image load_sif(char *image_path) {

	image img = { 0 };

	size_t file_size;
	char *image_data = read_entire_file_with_mmap(image_path, &file_size);

	char *data = image_data;
	sif_header header = *((sif_header*) data);
	data += sizeof(header);

	img.height = header.height;
	img.width = header.width;
	
	if(!header.has_alpha) {
		img.pixel_format = UR8G8B8;
	}
	else {
		img.pixel_format = UR8G8B8A8;
	}

	uint64_t data_size = header.width*header.height*(header.bits_per_pixel/8);
	img.data = malloc(data_size);

	if(!header.compressed) {
		memcpy(img.data, data, data_size);	
	}
	else {
		//IMAGE FORMAT NOW
		//[#header]
		//[#blocks][#u-size][#p-size][#c-size-1][#c-size-2]...[#c-size-#blocks][DATA]
		//[#blocks] = Number of blocks
		//[#u-size] = Block size before compression
		//[#p-size] = Size of last partial block (zero if it not needed)
		//[#c-size-i] = Size in bytes of block i after compression

		uint64_t num_blocks = *((uint64_t*)data);
		data += sizeof(uint64_t);

		uint64_t block_size_before_compression = *((uint64_t*)data);
		data += sizeof(uint64_t);

		uint64_t last_block_size = *((uint64_t*)data);
		data += sizeof(uint64_t);

		uint64_t *block_sizes_compressed = (uint64_t*)calloc(num_blocks, sizeof(uint64_t));

		memcpy(block_sizes_compressed, data, num_blocks*sizeof(uint64_t));
        data += sizeof(uint64_t)*num_blocks;

		get_data_block_from_compressed_image_stream(data, img.data, num_blocks, block_size_before_compression, 
											        last_block_size, block_sizes_compressed);
	}

	munmap(image_data, file_size);
	return img;

}

int write_image_as_sif(image img, char *save_path, bool compress) {

	CHECK_PATH(save_path, 0);

	sif_header header;
	memcpy(&header.start, "SIF", 3);
	
	header.width = img.width;
	header.height = img.height;
	header.compressed = compress;

	if(img.pixel_format == UR8G8B8) {
		header.bits_per_pixel = 24;
		header.has_alpha = 0;
	}

	uint64_t data_size = header.width*header.height*(header.bits_per_pixel/8);

	FILE *img_file = fopen(save_path, "wb");
	fwrite(&header, sizeof(header), 1, img_file);

	if(compress) {

		//COMPRESSED IMAGE FORMAT
		//[#header] 15 bytes
		//[#blocks][#u-size][#p-size][#c-size-1][#c-size-2]...[#c-size-#blocks][DATA]
		
		//[#blocks] = Number of blocks
		//[#u-size] = Block size before compression
		//[#p-size] = Size of last partial block (zero if it not needed)
		//[#c-size-i] = Size in bytes of block i after compression

		size_t size_after_compression;
		unsigned char *compressed_data = NULL;
		size_t num_blocks;
		size_t block_size_uncompressed;
		size_t *block_sizes_compressed;
		size_t last_block_size;

		calculate_blocks_and_compress_data(data_size, &size_after_compression, img.data, &compressed_data, &num_blocks, 
				                           &block_size_uncompressed, &block_sizes_compressed, &last_block_size, 6);

		fwrite(&num_blocks, sizeof(uint64_t), 1, img_file);
		fwrite(&block_size_uncompressed, sizeof(uint64_t), 1, img_file);
		fwrite(&last_block_size, sizeof(uint64_t), 1, img_file);
		fwrite(block_sizes_compressed, sizeof(uint64_t), num_blocks, img_file);
		fwrite(compressed_data, size_after_compression, 1, img_file);

		free(compressed_data);
		free(block_sizes_compressed);

	}
	else {
		fwrite(img.data, data_size, 1, img_file); 
	}

	fclose(img_file);

	return 1;

}

