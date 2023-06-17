#include "mandelbrot.h"



Mandelbrot::Mandelbrot(int width, int height, QObject* parent) :
    QObject(parent), WIDTH(width), HEIGHT(height) {
    QImage image(QSize(WIDTH, HEIGHT), QImage::Format_RGB888);
    m_image = image;

    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0) {
        qDebug() << "No platforms found.";
        exit(-1);
    }

    cl::Platform default_platform = all_platforms[0];
    qDebug() << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>();

    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.size() == 0) {
        qDebug() << "No devices found.";
        exit(-1);
    }

    cl::Device default_device = all_devices[0];
    qDebug() << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>();

    cl::Context context({default_device});

    std::vector<uchar> iterations(WIDTH * HEIGHT * 3);
    cl::Buffer iterations_d(context, CL_MEM_READ_WRITE, sizeof(uchar) * WIDTH * HEIGHT * 3);

    cl::CommandQueue queue(context, default_device);

    std::ifstream kernelFile("kernel.cl");
    if (!kernelFile.is_open()) {
        throw std::runtime_error("Failed to open kernel file");
    }
    std::string kernelSource((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
    kernelFile.close();

    cl::Program::Sources sources;
    sources.push_back({ kernelSource.c_str(), kernelSource.length() });
    cl::Program program(context, sources);

    if (program.build({default_device}) != CL_SUCCESS) {
        qDebug() << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device);
        exit(-1);
    }

    auto kernel = cl::Kernel(program, "mandelbrot");

    m_iterations_buffer = iterations_d;
    m_kernel = kernel;
    m_queue = queue;
    m_iterations = iterations;
}


void Mandelbrot::update(double zoom, double offset_x, double offset_y) {
    uchar* image = new uchar[WIDTH * HEIGHT * 3];

    m_kernel.setArg(0, m_iterations_buffer);
    m_kernel.setArg(1, WIDTH);
    m_kernel.setArg(2, HEIGHT);
    m_kernel.setArg(3, DEPTH);
    m_kernel.setArg(4, zoom);
    m_kernel.setArg(5, offset_x);
    m_kernel.setArg(6, offset_y);


    try {
        m_queue.enqueueNDRangeKernel(m_kernel,
                                     cl::NullRange,
                                     cl::NDRange(WIDTH * HEIGHT),
                                     cl::NullRange);
    } catch (cl::Error& e) {
        qDebug() << "Error during kernel execution: "
                 << e.what()
                 << "(" << e.err() << ")";
    }

    m_queue.enqueueReadBuffer(m_iterations_buffer,
                              CL_TRUE,
                              0,
                              sizeof(uchar) * WIDTH * HEIGHT * 3,
                              m_iterations.data());

    int index;
    int x;
    int y;

    m_image = QImage(m_iterations.data(), WIDTH, HEIGHT, QImage::Format::Format_RGB888);

//    for (int id = 0; id < WIDTH * HEIGHT; id++) {
//        col = Mandelbrot::color(m_iterations[id]);
//        x = id % WIDTH;
//        y = id / WIDTH;
//        index = 3 * (WIDTH * y + x);
//        image[index] = m_iterations[id];//col.red();
//        image[index + 1] = m_iterations[id];//col.green();
//        image[index + 2] = m_iterations[id];//col.blue();
//    }
//    m_image = QImage(image, WIDTH, HEIGHT, QImage::Format_RGB888);
    emit imageChanged(m_image);
//    return;

//    const int ROWS_PER_THREAD = HEIGHT / std::thread::hardware_concurrency();
//    std::vector<std::thread> threads;
//    for (int i = 0; i < HEIGHT; i += ROWS_PER_THREAD) {
//        threads.push_back(std::thread(&Mandelbrot::update_slice,
//                                      this,
//                                      zoom,
//                                      offset_x,
//                                      offset_y,
//                                      std::ref(image),
//                                      i,
//                                      std::min(i + ROWS_PER_THREAD, HEIGHT))
//                          );
//    }
//    for (auto& t : threads) {
//        t.join();
//    }

//    m_image = QImage(image, WIDTH, HEIGHT, QImage::Format_RGB888);
//    emit imageChanged(m_image);
}


int Mandelbrot::width(void) const {
    return WIDTH;
}


int Mandelbrot::height(void) const {
    return HEIGHT;
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

    double real_old = 0;
    double imag_old = 0;
    int period = 0;

    double cardioid_param = (real - 0.25) * (real - 0.25);
    cardioid_param += imag * imag;
    cardioid_param = 4.0 * cardioid_param * (cardioid_param + (real - 0.25));
    if (cardioid_param <= imag * imag) return DEPTH;

    for (int iterations = 0; iterations < DEPTH; ++iterations) {
        r2 = real * real;
        i2 = imag * imag;
        if (r2 + i2 > DIVERGENCE_BOUND_SQ) {
            return iterations;
        }
        imag *= 2.0 * real;
        imag += imag0;
        real = r2 - i2 + real0;
        if (std::fabs(real - real_old) < PERIODICITY_EPSILON
            && std::fabs(imag - imag_old < PERIODICITY_EPSILON)) break;
        period++;
        if (period == MAX_PERIOD) {
            period = 0;
            real_old = real;
            imag_old = imag;
        }
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
