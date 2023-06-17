#include "mandelbrot.h"

#include <thread>

#include <QDebug>

Mandelbrot::Mandelbrot(int width, int height, QObject* parent) :
    QObject(parent), WIDTH(width), HEIGHT(height) {
    QImage image(QSize(WIDTH, HEIGHT), QImage::Format_RGB888);
    m_image = image;
}


void Mandelbrot::update(double zoom, double offset_x, double offset_y) {
    uchar* image = new uchar[WIDTH * HEIGHT * 3];

    const int ROWS_PER_THREAD = HEIGHT / std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (int i = 0; i < HEIGHT; i += ROWS_PER_THREAD) {
        threads.push_back(std::thread(&Mandelbrot::update_slice,
                                      this,
                                      zoom,
                                      offset_x,
                                      offset_y,
                                      std::ref(image),
                                      i,
                                      std::min(i + ROWS_PER_THREAD, HEIGHT))
                          );
    }
    for (auto& t : threads) {
        t.join();
    }

    m_image = QImage(image, WIDTH, HEIGHT, QImage::Format_RGB888);
    emit imageChanged(m_image);
}


int Mandelbrot::width(void) {
    return WIDTH;
}


int Mandelbrot::height(void) {
    return HEIGHT;
}


void Mandelbrot::update(void) {
    qDebug() << "This is apparently beign called";
    emit imageChanged(m_image);
}


void Mandelbrot::update_slice(double zoom, double offset_x, double offset_y,
                        uchar* image, int min_y, int max_y) {
    double real = -0.5 * WIDTH * zoom + offset_x;
    double imag0 = min_y * zoom - 0.5 * HEIGHT * zoom + offset_y;

    int iterations;
    QColor col;
    int index;
    double imag;

    for (int x = 0; x < WIDTH; x++, real += zoom) {
        imag = imag0;
        for (int y = min_y; y < max_y; y++, imag += zoom) {
            iterations = Mandelbrot::mandelbrot(real, imag);
            col = Mandelbrot::color(iterations);
            index = 3 * (WIDTH * y + x);
            image[index] = col.red();
            image[index + 1] = col.green();
            image[index + 2] = col.blue();
        }
    }
}


int Mandelbrot::mandelbrot(double real, double imag) {
    double real0 = real;
    double imag0 = imag;

    double r2;
    double i2;

    for (int iterations = 0; iterations < DEPTH; ++iterations) {
        r2 = real * real;
        i2 = imag * imag;
        if (r2 + i2 > DIVERGENCE_BOUND_SQ) {
            return iterations;
        }
        imag *= 2.0 * real;
        imag += imag0;
        real = r2 - i2 + real0;
    }
    return DEPTH;
}


QColor Mandelbrot::color(int iterations) {
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
    return QColor(r, g, b);
}
