#ifndef PHOTO_EDITOR_PGM_H
#define PHOTO_EDITOR_PGM_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    size_t width;
    size_t height;
    uint16_t max_value;
    uint16_t *data;
} PGMImage;

PGMImage pgm_create(size_t width, size_t height, uint16_t max_value);
void pgm_free(PGMImage *image);
int pgm_load_ascii(const char *path, PGMImage *out_image);
int pgm_save_ascii(const char *path, const PGMImage *image);
uint16_t pgm_get_pixel(const PGMImage *image, size_t row, size_t column);
void pgm_set_pixel(PGMImage *image, size_t row, size_t column, uint16_t value);
void pgm_print_summary(const PGMImage *image, const char *name);

#endif
