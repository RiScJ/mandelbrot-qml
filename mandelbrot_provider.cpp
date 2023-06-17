#include "mandelbrot_provider.h"


QImage MandelbrotProvider::requestImage(const QString& id, QSize* size,
                                        const QSize& requestedSize) {
    return m_image;
}


void MandelbrotProvider::updateImage(QImage image) {
    m_image = image;
}
