#ifndef MANDELBROT_PROVIDER_H
#define MANDELBROT_PROVIDER_H

#include <QObject>
#include <QQuickImageProvider>

class MandelbrotProvider : public QQuickImageProvider {
public:
    MandelbrotProvider(void) : QQuickImageProvider(QQuickImageProvider::Image)
        {};
    QImage requestImage(const QString& id, QSize* size,
                        const QSize& requestedSize) override;

public slots:
    void updateImage(QImage image);

private:
    QImage m_image;
};

#endif // MANDELBROT_PROVIDER_H
