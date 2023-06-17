cmake_minimum_required(VERSION 3.16)

project(mandelbrot-qml VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH
    /home/riscj/OpenCL-Headers
    /home/riscj/OpenCL-ICD-Loader/build
    /home/riscj/OpenCL-CLHPP/build
)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)
find_package(OpenCLHeaders REQUIRED)
find_package(OpenCLICDLoader REQUIRED)
find_package(OpenCLHeadersCpp REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appmandelbrot-qml
    main.cpp
    mandelbrot.h mandelbrot.cpp
    mandelbrot_provider.h mandelbrot_provider.cpp
)

qt_add_qml_module(appmandelbrot-qml
    URI mandelbrot-qml
    VERSION 1.0
    QML_FILES Main.qml
)

file(COPY kernel.cl DESTINATION ${CMAKE_BINARY_DIR})

set_target_properties(appmandelbrot-qml PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appmandelbrot-qml
    PRIVATE Qt6::Quick
    PRIVATE OpenCL::Headers
    PRIVATE OpenCL::OpenCL
    PRIVATE OpenCL::HeadersCpp
)

install(TARGETS appmandelbrot-qml
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
