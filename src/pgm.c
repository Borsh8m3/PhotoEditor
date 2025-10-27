#include "pgm.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PGM_MAGIC "P2"

PGMImage pgm_create(size_t width, size_t height, uint16_t max_value)
{
    PGMImage image = {width, height, max_value, NULL};
    if (width == 0 || height == 0)
    {
        return image;
    }

    size_t pixel_count = width * height;
    image.data = calloc(pixel_count, sizeof(uint16_t));
    return image;
}

void pgm_free(PGMImage *image)
{
    if (image == NULL)
    {
        return;
    }

    free(image->data);
    image->data = NULL;
    image->width = 0;
    image->height = 0;
    image->max_value = 0;
}

static int read_token(FILE *stream, char *buffer, size_t buffer_size)
{
    int c;

    // Skip whitespace and comments.
    while ((c = fgetc(stream)) != EOF)
    {
        if (c == '#')
        {
            while ((c = fgetc(stream)) != EOF && c != '\n')
            {
            }
        }
        else if (!isspace(c))
        {
            break;
        }
    }

    if (c == EOF)
    {
        return EOF;
    }

    size_t i = 0;
    do
    {
        if (i + 1 < buffer_size)
        {
            buffer[i++] = (char)c;
        }
        c = fgetc(stream);
    } while (c != EOF && !isspace(c));

    buffer[i] = '\0';
    return 0;
}

int pgm_load_ascii(const char *path, PGMImage *out_image)
{
    if (out_image == NULL)
    {
        return EINVAL;
    }

    memset(out_image, 0, sizeof(*out_image));

    FILE *file = fopen(path, "r");
    if (!file)
    {
        return errno ? errno : EIO;
    }

    char token[64];
    if (read_token(file, token, sizeof(token)) == EOF || strcmp(token, PGM_MAGIC) != 0)
    {
        fclose(file);
        return EINVAL;
    }

    if (read_token(file, token, sizeof(token)) == EOF)
    {
        fclose(file);
        return EINVAL;
    }
    size_t width = strtoul(token, NULL, 10);

    if (read_token(file, token, sizeof(token)) == EOF)
    {
        fclose(file);
        return EINVAL;
    }
    size_t height = strtoul(token, NULL, 10);

    if (read_token(file, token, sizeof(token)) == EOF)
    {
        fclose(file);
        return EINVAL;
    }
    long max_value_long = strtol(token, NULL, 10);
    if (max_value_long <= 0 || max_value_long > UINT16_MAX)
    {
        fclose(file);
        return EINVAL;
    }

    PGMImage image = pgm_create(width, height, (uint16_t)max_value_long);
    if (!image.data)
    {
        fclose(file);
        return ENOMEM;
    }

    size_t pixel_count = width * height;
    for (size_t i = 0; i < pixel_count; ++i)
    {
        if (read_token(file, token, sizeof(token)) == EOF)
        {
            pgm_free(&image);
            fclose(file);
            return EINVAL;
        }

        long value = strtol(token, NULL, 10);
        if (value < 0 || value > image.max_value)
        {
            pgm_free(&image);
            fclose(file);
            return EINVAL;
        }

        image.data[i] = (uint16_t)value;
    }

    fclose(file);
    *out_image = image;
    return 0;
}

int pgm_save_ascii(const char *path, const PGMImage *image)
{
    if (!image || !image->data)
    {
        return EINVAL;
    }

    FILE *file = fopen(path, "w");
    if (!file)
    {
        return errno ? errno : EIO;
    }

    fprintf(file, "%s\n", PGM_MAGIC);
    fprintf(file, "%zu %zu\n", image->width, image->height);
    fprintf(file, "%u\n", image->max_value);

    size_t pixel_count = image->width * image->height;
    for (size_t i = 0; i < pixel_count; ++i)
    {
        fprintf(file, "%u", image->data[i]);
        if ((i + 1) % image->width == 0)
        {
            fputc('\n', file);
        }
        else
        {
            fputc(' ', file);
        }
    }

    fclose(file);
    return 0;
}

uint16_t pgm_get_pixel(const PGMImage *image, size_t row, size_t column)
{
    return image->data[row * image->width + column];
}

void pgm_set_pixel(PGMImage *image, size_t row, size_t column, uint16_t value)
{
    image->data[row * image->width + column] = value;
}

void pgm_print_summary(const PGMImage *image, const char *name)
{
    if (!image)
    {
        return;
    }

    printf("%s\n", name ? name : "(unnamed image)");
    printf("  Size: %zu x %zu\n", image->width, image->height);
    printf("  Value range: 0-%u\n", image->max_value);

    if (image->data)
    {
        uint16_t min = image->data[0];
        uint16_t max = image->data[0];
        unsigned long long sum = 0ULL;
        size_t pixel_count = image->width * image->height;
        for (size_t i = 0; i < pixel_count; ++i)
        {
            uint16_t value = image->data[i];
            if (value < min)
            {
                min = value;
            }
            if (value > max)
            {
                max = value;
            }
            sum += value;
        }
        double average = pixel_count ? (double)sum / (double)pixel_count : 0.0;
        printf("  Pixel stats -> min: %u, max: %u, avg: %.2f\n", min, max, average);
    }
}
