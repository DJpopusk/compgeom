#include <QApplication>

#include "main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Task12MainWindow w;
    w.resize(960, 720);
    w.show();
    return app.exec();
}

