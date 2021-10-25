#ifndef _CHAR_ARRAY_H_
#define _CHAR_ARRAY_H_

typedef struct {
    unsigned char* array;
    size_t used;
    size_t size;
} UnsignedCharArray;

void initUnsignedCharArray(UnsignedCharArray* a, size_t initialSize) {
    a->array = (unsigned char*)malloc(initialSize * sizeof(unsigned char));
    a->used = 0;
    a->size = initialSize;
}

void insertUnsignedCharArray(UnsignedCharArray* a, unsigned char element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size 
    if (a->used == a->size) {
        a->size *= 2;
        unsigned char* reallocated = (unsigned char*)realloc(a->array, a->size * sizeof(unsigned char));
        a->array = reallocated;
    }
    a->array[a->used++] = element;
}

void freeUnsignedCharArray(UnsignedCharArray* a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

#endif // !_CHAR_ARRAY_H_
