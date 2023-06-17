#include <QGuiApplication>
#include <QWindow>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "mandelbrot.h"
#include "mandelbrot_provider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (const QWindow* window = qApp->focusWindow()) screen = window->screen();
    int screen_width = screen->geometry().width();
    int screen_height = screen->geometry().height();

    Mandelbrot* mb = new Mandelbrot(screen_width, screen_height);
    engine.rootContext()->setContextProperty("mb", mb);

    MandelbrotProvider* mb_provider = new MandelbrotProvider();
    QObject::connect(mb, &Mandelbrot::imageChanged,
                     mb_provider, &MandelbrotProvider::updateImage);
    mb->update(0.004, 0, 0);
    engine.addImageProvider("mb_provider", mb_provider);

    engine.loadFromModule("mandelbrot-qml", "Main");
    return app.exec();
}
