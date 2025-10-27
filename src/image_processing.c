#include "image_processing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    size_t width;
    size_t height;
} Dimensions;

static Dimensions rotated_dimensions(const PGMImage *image, unsigned int quarter_turns)
{
    if (quarter_turns % 2 == 0)
    {
        Dimensions dimensions = {image->width, image->height};
        return dimensions;
    }

    Dimensions dimensions = {image->height, image->width};
    return dimensions;
}

Histogram histogram_create(const PGMImage *image)
{
    Histogram histogram = {0, NULL};
    if (!image || !image->data)
    {
        return histogram;
    }

    histogram.histogram_size = (size_t)image->max_value + 1;
    histogram.values = calloc(histogram.histogram_size, sizeof(unsigned long long));
    if (!histogram.values)
    {
        histogram.histogram_size = 0;
        return histogram;
    }

    size_t pixel_count = image->width * image->height;
    for (size_t i = 0; i < pixel_count; ++i)
    {
        uint16_t value = image->data[i];
        histogram.values[value]++;
    }

    return histogram;
}

void histogram_free(Histogram *histogram)
{
    if (!histogram)
    {
        return;
    }

    free(histogram->values);
    histogram->values = NULL;
    histogram->histogram_size = 0;
}

int histogram_save_csv(const Histogram *histogram, uint16_t max_value, const char *path)
{
    if (!histogram || !histogram->values)
    {
        return -1;
    }

    FILE *file = fopen(path, "w");
    if (!file)
    {
        return -1;
    }

    fprintf(file, "value,count\n");
    for (size_t i = 0; i < histogram->histogram_size && i <= max_value; ++i)
    {
        fprintf(file, "%zu,%llu\n", i, histogram->values[i]);
    }

    fclose(file);
    return 0;
}

static unsigned int next_random(unsigned int *state)
{
    // Xorshift32 for repeatable deterministic noise.
    unsigned int x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

void pgm_rotate_left(PGMImage *image, unsigned int quarter_turns)
{
    if (!image || !image->data)
    {
        return;
    }

    quarter_turns = quarter_turns % 4;
    if (quarter_turns == 0)
    {
        return;
    }

    Dimensions dims = rotated_dimensions(image, quarter_turns);
    PGMImage rotated = pgm_create(dims.width, dims.height, image->max_value);
    if (!rotated.data)
    {
        return;
    }

    for (size_t r = 0; r < image->height; ++r)
    {
        for (size_t c = 0; c < image->width; ++c)
        {
            uint16_t value = pgm_get_pixel(image, r, c);
            size_t new_row;
            size_t new_col;
            switch (quarter_turns)
            {
            case 1:
                new_row = (image->width - 1) - c;
                new_col = r;
                break;
            case 2:
                new_row = (image->height - 1) - r;
                new_col = (image->width - 1) - c;
                break;
            case 3:
            default:
                new_row = c;
                new_col = (image->height - 1) - r;
                break;
            }
            rotated.data[new_row * rotated.width + new_col] = value;
        }
    }

    pgm_free(image);
    *image = rotated;
}

void pgm_apply_threshold(PGMImage *image, uint16_t threshold)
{
    if (!image || !image->data)
    {
        return;
    }

    size_t pixel_count = image->width * image->height;
    for (size_t i = 0; i < pixel_count; ++i)
    {
        image->data[i] = image->data[i] >= threshold ? image->max_value : 0;
    }
}

void pgm_add_salt_and_pepper_noise(PGMImage *image, double probability, unsigned int *state)
{
    if (!image || !image->data || probability <= 0.0)
    {
        return;
    }

    if (probability > 1.0)
    {
        probability = 1.0;
    }

    if (!state)
    {
        unsigned int temp_state = 0x12345678u;
        state = &temp_state;
    }

    size_t pixel_count = image->width * image->height;
    for (size_t i = 0; i < pixel_count; ++i)
    {
        double random_value = (double)(next_random(state) & 0xFFFFFF) / (double)0xFFFFFF;
        if (random_value < probability / 2.0)
        {
            image->data[i] = 0;
        }
        else if (random_value > 1.0 - probability / 2.0)
        {
            image->data[i] = image->max_value;
        }
    }
}

static uint16_t median_of_block(const uint16_t *values, size_t size)
{
    uint16_t *buffer = malloc(size * sizeof(uint16_t));
    if (!buffer)
    {
        return values[size / 2];
    }

    memcpy(buffer, values, size * sizeof(uint16_t));
    for (size_t i = 0; i + 1 < size; ++i)
    {
        for (size_t j = i + 1; j < size; ++j)
        {
            if (buffer[j] < buffer[i])
            {
                uint16_t tmp = buffer[i];
                buffer[i] = buffer[j];
                buffer[j] = tmp;
            }
        }
    }

    uint16_t median = buffer[size / 2];
    free(buffer);
    return median;
}

void pgm_apply_median_filter(PGMImage *image)
{
    if (!image || !image->data || image->width < 3 || image->height < 3)
    {
        return;
    }

    PGMImage result = pgm_create(image->width, image->height, image->max_value);
    if (!result.data)
    {
        return;
    }

    const size_t kernel_size = 3;
    uint16_t window[kernel_size * kernel_size];

    for (size_t r = 1; r + 1 < image->height; ++r)
    {
        for (size_t c = 1; c + 1 < image->width; ++c)
        {
            size_t index = 0;
            for (int kr = -1; kr <= 1; ++kr)
            {
                for (int kc = -1; kc <= 1; ++kc)
                {
                    window[index++] = pgm_get_pixel(image, (size_t)((int)r + kr), (size_t)((int)c + kc));
                }
            }
            pgm_set_pixel(&result, r, c, median_of_block(window, kernel_size * kernel_size));
        }
    }

    // Copy border pixels without modification.
    for (size_t c = 0; c < image->width; ++c)
    {
        pgm_set_pixel(&result, 0, c, pgm_get_pixel(image, 0, c));
        pgm_set_pixel(&result, image->height - 1, c, pgm_get_pixel(image, image->height - 1, c));
    }
    for (size_t r = 0; r < image->height; ++r)
    {
        pgm_set_pixel(&result, r, 0, pgm_get_pixel(image, r, 0));
        pgm_set_pixel(&result, r, image->width - 1, pgm_get_pixel(image, r, image->width - 1));
    }

    pgm_free(image);
    *image = result;
}
