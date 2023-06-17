#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void mandelbrot(__global uchar* image, int WIDTH, int HEIGHT,
                         int DEPTH, double zoom, double offset_x, double offset_y) {
    int id = get_global_id(0);
    if (id >= WIDTH * HEIGHT) return;
    int x = id % WIDTH;
    int y = id / WIDTH;

    double real = -0.5 * WIDTH * zoom + offset_x;
    double imag = -0.5 * HEIGHT * zoom + offset_y;

    real = real + (zoom * x);
    imag = imag + (zoom * y);


    double real0 = real;
    double imag0 = imag;

    double r2;
    double i2;

    int iterations = DEPTH;
    for (int i = 0; i < DEPTH; i++) {
        r2 = real * real;
        i2 = imag * imag;
        if (r2 + i2 > 4.0) {
            iterations = i;
            break;
        }
        imag *= 2.0 * real;
        imag += imag0;
        real = r2 - i2 + real0;
    }

    int r, g, b;

    if (iterations == -1) {
        r = 0;
        g = 0;
        b = 0;
    } else if (iterations == 0) {
        r = 255;
        g = 0;
        b = 0;
    } else {
        if (iterations < 16) {
            r = 16 * (16 - iterations);
            g = 0;
            b = 16 * iterations - 1;
        } else if (iterations < 32) {
            r = 0;
            g = 16 * (iterations - 16);
            b = 16 * (32 - iterations) - 1;
        } else if (iterations < 64) {
            r = 8 * (iterations - 32);
            g = 8 * (64 - iterations) - 1;
            b = 0;
        } else {
            r = 255 - (iterations - 64) * 4;
            g = 0;
            b = 0;
        }
    }

    int index = 3 * (WIDTH * y + x);
    image[index] = r;
    image[index + 1] = g;
    image[index + 2] = b;
}
