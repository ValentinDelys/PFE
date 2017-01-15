#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include <opencv.hpp>
#include <QMainWindow>


class PreProcessing : public QMainWindow
{
public:
    PreProcessing();
    void startPreProcessing();
    void houghCircles();
    void fitEllipse();
    int matchTemplate();

private:
    std::string filename;
};


#endif // PREPROCESSING_H
