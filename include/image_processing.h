#ifndef PHOTO_EDITOR_IMAGE_PROCESSING_H
#define PHOTO_EDITOR_IMAGE_PROCESSING_H

#include "pgm.h"

#include <stddef.h>

typedef struct
{
    size_t histogram_size;
    unsigned long long *values;
} Histogram;

Histogram histogram_create(const PGMImage *image);
void histogram_free(Histogram *histogram);
int histogram_save_csv(const Histogram *histogram, uint16_t max_value, const char *path);

void pgm_rotate_left(PGMImage *image, unsigned int quarter_turns);
void pgm_apply_threshold(PGMImage *image, uint16_t threshold);
void pgm_add_salt_and_pepper_noise(PGMImage *image, double probability, unsigned int *state);
void pgm_apply_median_filter(PGMImage *image);

#endif
