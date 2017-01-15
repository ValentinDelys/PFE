#-------------------------------------------------
#
# Project created by QtCreator 2016-09-26T13:51:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iPIX
TEMPLATE = app

MOC_DIR     = .moc
OBJECTS_DIR = .obj

CONFIG          += warn_on silent c++11

OPENCV_DIR      = "../opencv/"

SOURCES += main.cpp\
        mainwindow.cpp \
    preprocessing.cpp

HEADERS  += mainwindow.h \
    preprocessing.h \
    gpxerrors.h \
    gpxerrors.h

INCLUDEPATH +=  $${OPENCV_DIR}include/opencv \
                $${OPENCV_DIR}include/opencv2 \
                $${OPENCV_DIR}modules/core/include \
                $${OPENCV_DIR}modules/imgproc/include \
                $${OPENCV_DIR}modules/photo/include \
                $${OPENCV_DIR}modules/video/include \
                $${OPENCV_DIR}modules/videoio/include \
                $${OPENCV_DIR}modules/features2d/include \
                $${OPENCV_DIR}modules/objdetect/include \
                $${OPENCV_DIR}modules/calib3d/include \
                $${OPENCV_DIR}modules/highgui/include \
                $${OPENCV_DIR}modules/imgcodecs/include \
                $${OPENCV_DIR}modules/contrib/include \
                $${OPENCV_DIR}modules/ml/include \
                $${OPENCV_DIR}modules/flann/include \
                $${OPENCV_DIR}modules/legacy/include \

CONFIG(release, debug|release){
    win32: LIBS += -L"$${OPENCV_DIR}/build/bin" -llibopencv_world310
}
else{
    win32: LIBS += -L"$${OPENCV_DIR}/build/bin" -llibopencv_world310d
}

FORMS    += mainwindow.ui
