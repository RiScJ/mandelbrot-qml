#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <QObject>
#include <QScreen>
#include <QImage>

class Mandelbrot : public QObject {
    Q_OBJECT
public:
    explicit Mandelbrot(QScreen screen, QObject* parent = nullptr);
    Q_INVOKABLE void update(double zoom, double offset_x, double offset_y);
    Q_INVOKABLE int width(void);
    Q_INVOKABLE int height(void);

signals:
    void imageChanged(QImage image);

private:
    void update(void);
    void update_slice(double zoom, double offset_x, double offset_y,
                      uchar* image, int min_y, int max_y);
    int mandelbrot(double real, double imag);
    QColor color(int iterations);

    QImage m_image;

    const int WIDTH;
    const int HEIGHT;
    constexpr static int DEPTH = 127;
    constexpr static double DIVERGENCE_BOUND_SQ = 4.0;
};

#endif // MANDELBROT_H
