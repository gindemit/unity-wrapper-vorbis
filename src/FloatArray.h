#ifndef _FLOAT_ARRAY_H_
#define _FLOAT_ARRAY_H_

typedef struct {
    float* array;
    size_t used;
    size_t size;
} FloatArray;

void initFloatArray(FloatArray* a, size_t initialSize) {
    a->array = (float*)malloc(initialSize * sizeof(float));
    a->used = 0;
    a->size = initialSize;
}

void insertFloatArray(FloatArray* a, float element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size 
    if (a->used == a->size) {
        a->size *= 2;
        float* reallocated = (float*)realloc(a->array, a->size * sizeof(float));
        a->array = reallocated;
    }
    a->array[a->used++] = element;
}

void freeFloatArray(FloatArray* a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

#endif // !_FLOAT_ARRAY_H_
