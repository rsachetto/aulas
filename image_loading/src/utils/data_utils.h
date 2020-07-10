//
// Created by sachetto on 01/11/18.
//

#ifndef MONOALG3D_DATA_UTILS_H
#define MONOALG3D_DATA_UTILS_H

#include <zlib.h>
#include <stdint.h>

int invert_bytes(int data);

size_t uncompress_buffer(unsigned char const* compressed_data,
                         size_t compressed_size,
                         unsigned char* uncompressed_data,
                         size_t uncompressed_size);

static size_t compress_buffer(unsigned char const *uncompressed_data, size_t uncompressed_data_size,
                              unsigned char *compressed_data, size_t compression_space, int level);

void calculate_blocks_and_compress_data(size_t total_data_size_before_compression,
                                        size_t *total_data_size_after_compression, unsigned char *data_to_compress,
                                        unsigned char **compressed_buffer, size_t *num_blocks,
                                        size_t *block_size_uncompressed, size_t **block_sizes_compressed,
                                        size_t *last_block_size, int compression_level);

size_t get_block_sizes_from_compressed_vtu_file(char *raw_data, size_t header_size, uint64_t *num_blocks, uint64_t *block_size_uncompressed, uint64_t *last_block_size, uint64_t  **block_sizes_compressed);
void get_data_block_from_compressed_image_stream(char *raw_data, void* values, uint64_t num_blocks, uint64_t block_size_uncompressed, uint64_t last_block_size, uint64_t  *block_sizes_compressed);

#endif // MONOALG3D_DATA_UTILS_H
