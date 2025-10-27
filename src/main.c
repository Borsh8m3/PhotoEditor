#include "image_processing.h"
#include "library.h"
#include "pgm.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII_PREVIEW_WIDTH 80

static void trim_newline(char *buffer)
{
    if (!buffer)
    {
        return;
    }

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n')
    {
        buffer[length - 1] = '\0';
    }
}

static bool prompt_string(const char *message, char *buffer, size_t size)
{
    printf("%s", message);
    if (!fgets(buffer, (int)size, stdin))
    {
        return false;
    }
    trim_newline(buffer);
    return strlen(buffer) > 0;
}

static long prompt_long(const char *message, long min, long max)
{
    char buffer[128];
    while (true)
    {
        if (!prompt_string(message, buffer, sizeof(buffer)))
        {
            continue;
        }

        char *end = NULL;
        long value = strtol(buffer, &end, 10);
        if (end && *end == '\0' && value >= min && value <= max)
        {
            return value;
        }

        printf("Please enter a value between %ld and %ld.\n", min, max);
    }
}

static double prompt_double(const char *message, double min, double max)
{
    char buffer[128];
    while (true)
    {
        if (!prompt_string(message, buffer, sizeof(buffer)))
        {
            continue;
        }

        char *end = NULL;
        double value = strtod(buffer, &end);
        if (end && *end == '\0' && value >= min && value <= max)
        {
            return value;
        }

        printf("Please enter a value between %.2f and %.2f.\n", min, max);
    }
}

static void print_separator(void)
{
    puts("------------------------------------------------------------");
}

static void print_ascii_preview(const PGMImage *image)
{
    if (!image || !image->data)
    {
        printf("Image data is empty.\n");
        return;
    }

    const char *palette = " .:-=+*#%@";
    size_t palette_length = strlen(palette) - 1;

    size_t column_step = image->width > ASCII_PREVIEW_WIDTH ? (image->width + ASCII_PREVIEW_WIDTH - 1) / ASCII_PREVIEW_WIDTH : 1;
    size_t row_step = column_step;

    for (size_t row = 0; row < image->height; row += row_step)
    {
        for (size_t column = 0; column < image->width; column += column_step)
        {
            uint16_t value = pgm_get_pixel(image, row, column);
            double normalized = image->max_value == 0 ? 0.0 : (double)value / (double)image->max_value;
            size_t index = (size_t)(normalized * palette_length);
            if (index > palette_length)
            {
                index = palette_length;
            }
            putchar(palette[index]);
        }
        putchar('\n');
    }
}

static void print_menu(void)
{
    print_separator();
    puts("PGM Photo Editor");
    puts("1. Load image");
    puts("2. Remove image");
    puts("3. List images");
    puts("4. Edit image");
    puts("5. Exit");
}

static void print_edit_menu(void)
{
    print_separator();
    puts("Edit menu");
    puts("1. Show summary");
    puts("2. ASCII preview");
    puts("3. Rotate 90° left (k times)");
    puts("4. Save histogram to CSV");
    puts("5. Apply threshold");
    puts("6. Add salt & pepper noise");
    puts("7. Apply median filter");
    puts("8. Save image");
    puts("9. Back to main menu");
}

static void handle_histogram(NamedImage *image)
{
    char path[256];
    if (!prompt_string("Output CSV path: ", path, sizeof(path)))
    {
        return;
    }

    Histogram histogram = histogram_create(&image->image);
    if (!histogram.values)
    {
        printf("Failed to create histogram.\n");
        return;
    }

    if (histogram_save_csv(&histogram, image->image.max_value, path) == 0)
    {
        printf("Histogram saved to %s\n", path);
    }
    else
    {
        printf("Failed to save histogram to %s\n", path);
    }

    histogram_free(&histogram);
}

static void handle_threshold(NamedImage *image)
{
    long threshold = prompt_long("Threshold value: ", 0, image->image.max_value);
    pgm_apply_threshold(&image->image, (uint16_t)threshold);
    puts("Threshold applied.");
}

static void handle_noise(NamedImage *image)
{
    double probability = prompt_double("Noise probability (0.0 - 1.0): ", 0.0, 1.0);
    unsigned int state = 0xC0FFEEu;
    pgm_add_salt_and_pepper_noise(&image->image, probability, &state);
    puts("Noise added.");
}

static void handle_rotate(NamedImage *image)
{
    long turns = prompt_long("Number of quarter turns to the left: ", 0, 12);
    pgm_rotate_left(&image->image, (unsigned int)turns);
    puts("Image rotated.");
}

static void handle_median(NamedImage *image)
{
    pgm_apply_median_filter(&image->image);
    puts("Median filter applied.");
}

static void handle_save(NamedImage *image)
{
    char path[256];
    if (!prompt_string("Save image as: ", path, sizeof(path)))
    {
        return;
    }

    int status = pgm_save_ascii(path, &image->image);
    if (status == 0)
    {
        printf("Image saved to %s\n", path);
    }
    else
    {
        printf("Failed to save image (%s).\n", strerror(status));
    }
}

static void edit_image(ImageLibrary *library)
{
    if (library->count == 0)
    {
        puts("No images loaded.");
        return;
    }

    for (size_t i = 0; i < library->count; ++i)
    {
        printf("%zu. %s\n", i + 1, library->items[i].name);
    }

    long index = prompt_long("Select image: ", 1, (long)library->count);
    NamedImage *image = library_get(library, (size_t)(index - 1));
    if (!image)
    {
        puts("Invalid selection.");
        return;
    }

    bool editing = true;
    while (editing)
    {
        print_edit_menu();
        long choice = prompt_long("Choose option: ", 1, 9);
        switch (choice)
        {
        case 1:
            pgm_print_summary(&image->image, image->name);
            break;
        case 2:
            print_ascii_preview(&image->image);
            break;
        case 3:
            handle_rotate(image);
            break;
        case 4:
            handle_histogram(image);
            break;
        case 5:
            handle_threshold(image);
            break;
        case 6:
            handle_noise(image);
            break;
        case 7:
            handle_median(image);
            break;
        case 8:
            handle_save(image);
            break;
        case 9:
            editing = false;
            break;
        default:
            break;
        }
    }
}

int main(void)
{
    ImageLibrary library;
    library_init(&library);

    bool running = true;
    while (running)
    {
        print_menu();
        long choice = prompt_long("Choose option: ", 1, 5);

        switch (choice)
        {
        case 1:
        {
            char path[256];
            if (!prompt_string("Path to PGM file: ", path, sizeof(path)))
            {
                break;
            }

            int status = library_load_image(&library, path);
            if (status == 0)
            {
                printf("Loaded %s\n", path);
            }
            else
            {
                printf("Failed to load %s (%s).\n", path, strerror(status));
            }
            break;
        }
        case 2:
        {
            if (library.count == 0)
            {
                puts("No images to remove.");
                break;
            }

            for (size_t i = 0; i < library.count; ++i)
            {
                printf("%zu. %s\n", i + 1, library.items[i].name);
            }
            long index = prompt_long("Select image to remove: ", 1, (long)library.count);
            if (library_remove_image(&library, (size_t)(index - 1)) == 0)
            {
                puts("Image removed.");
            }
            else
            {
                puts("Unable to remove image.");
            }
            break;
        }
        case 3:
        {
            if (library.count == 0)
            {
                puts("No images loaded.");
                break;
            }

            for (size_t i = 0; i < library.count; ++i)
            {
                pgm_print_summary(&library.items[i].image, library.items[i].name);
            }
            break;
        }
        case 4:
            edit_image(&library);
            break;
        case 5:
            running = false;
            break;
        default:
            break;
        }
    }

    library_free(&library);
    return 0;
}
