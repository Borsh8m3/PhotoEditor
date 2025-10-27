#include "library.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static char *duplicate_string(const char *value)
{
    size_t length = strlen(value) + 1;
    char *result = malloc(length);
    if (!result)
    {
        return NULL;
    }
    memcpy(result, value, length);
    return result;
}

void library_init(ImageLibrary *library)
{
    if (!library)
    {
        return;
    }

    library->items = NULL;
    library->count = 0;
    library->capacity = 0;
}

void library_free(ImageLibrary *library)
{
    if (!library)
    {
        return;
    }

    for (size_t i = 0; i < library->count; ++i)
    {
        free(library->items[i].name);
        pgm_free(&library->items[i].image);
    }
    free(library->items);
    library->items = NULL;
    library->count = 0;
    library->capacity = 0;
}

static int library_reserve(ImageLibrary *library, size_t capacity)
{
    if (capacity <= library->capacity)
    {
        return 0;
    }

    NamedImage *items = realloc(library->items, capacity * sizeof(NamedImage));
    if (!items)
    {
        return ENOMEM;
    }

    library->items = items;
    library->capacity = capacity;
    return 0;
}

int library_load_image(ImageLibrary *library, const char *path)
{
    if (!library || !path)
    {
        return EINVAL;
    }

    if (library->count == library->capacity)
    {
        size_t new_capacity = library->capacity == 0 ? 4 : library->capacity * 2;
        int result = library_reserve(library, new_capacity);
        if (result != 0)
        {
            return result;
        }
    }

    PGMImage image;
    int status = pgm_load_ascii(path, &image);
    if (status != 0)
    {
        return status;
    }

    char *name = duplicate_string(path);
    if (!name)
    {
        pgm_free(&image);
        return ENOMEM;
    }

    library->items[library->count].name = name;
    library->items[library->count].image = image;
    library->count += 1;
    return 0;
}

int library_remove_image(ImageLibrary *library, size_t index)
{
    if (!library || index >= library->count)
    {
        return EINVAL;
    }

    free(library->items[index].name);
    pgm_free(&library->items[index].image);

    for (size_t i = index; i + 1 < library->count; ++i)
    {
        library->items[i] = library->items[i + 1];
    }

    library->count -= 1;
    return 0;
}

NamedImage *library_get(ImageLibrary *library, size_t index)
{
    if (!library || index >= library->count)
    {
        return NULL;
    }

    return &library->items[index];
}
