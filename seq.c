#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#define H 800
#define wid 800
#define max 1000

typedef struct {
    double imaginary;
    double Real;
} complex;

double mod(complex num) {
    return (double) sqrt(num.Real*num.Real + num.imaginary*num.imaginary);
}

complex squared(complex numb) {
    complex numbsqrd;
    numbsqrd.Real = numb.Real*numb.Real - numb.imaginary*numb.imaginary;
    numbsqrd.imaginary = 2*numb.Real*numb.imaginary;
    return numbsqrd;
}

int mndlbrt(complex z) {
    complex a = {0, 0};
    for (int i = 0; i < max; i++) {
        a = squared(z);
        a.Real = a.Real + z.Real;
        a.imaginary =a.imaginary + z.imaginary;
        if (mod(z) > 2) {
            return 0;
        }
    }
    return 1;
}

complex mapping(int x, int y) {
    complex a;
    a.Real = (double)(x - wid/2) / (double)(wid/4);
    a.imaginary = (double)(y - H/2) / (double)(H/4);
    return a;
}

void generate_image(uint8_t *img) {
    int x, y;
    for (y = 0; y < H; y++) {
        for (x = 0; x < wid; x++) {
            complex a = mapping(x, y);
            if (mndlbrt(a)) {
                img[(y*wid + x)*3] = 0;
                img[(y*wid + x)*3 + 1] = 0;
                img[(y*wid + x)*3 + 2] = 0;
            } else {
                img[(y*wid + x)*3] = 255;
                img[(y*wid + x)*3 + 1] = 255;
                img[(y*wid + x)*3 + 2] = 255;
            }
        }
    }
}

int main() {
    clock_t start = clock();
    uint8_t *img = (uint8_t*) malloc(wid*H*3);
    generate_image(img);
    clock_t end = clock();
    
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time_spent);
    FILE *f = fopen("mandelbrot.ppm", "wb");
    fprintf(f, "P6\n%d %d\n255\n", 800, 800);
    fwrite(img, sizeof(uint8_t), wid*H*3, f);
    fclose(f);
    free(img);

    return 0;
}
