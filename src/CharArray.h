#ifndef _CHAR_ARRAY_H_
#define _CHAR_ARRAY_H_

typedef struct {
    char* array;
    size_t used;
    size_t size;
} CharArray;

void initArray(CharArray* a, size_t initialSize) {
    a->array = (char*)malloc(initialSize * sizeof(char));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(CharArray* a, char element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size 
    if (a->used == a->size) {
        a->size *= 2;
        char* reallocated = (char*)realloc(a->array, a->size * sizeof(char));
        a->array = reallocated;
    }
    a->array[a->used++] = element;
}

void freeArray(CharArray* a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

#endif // !_CHAR_ARRAY_H_
