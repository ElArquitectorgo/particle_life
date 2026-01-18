#include "base.h"

typedef struct {
    u8 rows, cols;
    f32* data;
} matrix;

matrix* mat_create(u8 rows, u8 cols);
void mat_clear(matrix* mat);
void mat_fill(matrix* mat, f32 x);
void mat_assign(matrix* mat, u8 row, u8 col, f32 value);
f32 mat_get(matrix* mat, u8 row, u8 col);
f32 get_rand();

f32 get_rand() {
    return (f32)rand() / RAND_MAX;
}

matrix* mat_create(u8 rows, u8 cols) {
    matrix* mat = malloc(sizeof(matrix));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = malloc(rows * cols * sizeof(f32));

    return mat;
}

void mat_clear(matrix* mat) {
    free(mat->data);
    free(mat);
}

void mat_fill(matrix* mat, f32 x) {
    u8 size = mat->rows * mat->cols;

    for (u8 i = 0; i < size; i++) {
        mat->data[i] = x;
    }
}

void mat_assign(matrix* mat, u8 row, u8 col, f32 value) {
    if (row >= mat->rows || col >= mat->cols) {
        return;
    }
    mat->data[row * mat->cols + col] = value;
}

f32 mat_get(matrix* mat, u8 row, u8 col) {
    if (row >= mat->rows || col >= mat->cols) {
        return 0.0f;
    }
    return mat->data[row * mat->cols + col];
}