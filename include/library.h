#ifndef PHOTO_EDITOR_LIBRARY_H
#define PHOTO_EDITOR_LIBRARY_H

#include "pgm.h"

#include <stddef.h>

typedef struct
{
    char *name;
    PGMImage image;
} NamedImage;

typedef struct
{
    NamedImage *items;
    size_t count;
    size_t capacity;
} ImageLibrary;

void library_init(ImageLibrary *library);
void library_free(ImageLibrary *library);
int library_load_image(ImageLibrary *library, const char *path);
int library_remove_image(ImageLibrary *library, size_t index);
NamedImage *library_get(ImageLibrary *library, size_t index);

#endif
