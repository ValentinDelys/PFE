#include "mainwindow.h"
#include "preprocessing.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    PreProcessing *preproc = new PreProcessing();

    /* Select a preprocessing methode */
//    preproc->houghCircles();
//    preproc->fitEllipseV3();
//    preproc->matchTemplate();
//    preproc->identification();

    return a.exec();
}


