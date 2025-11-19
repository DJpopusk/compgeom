#include <QApplication>
#include "canvas_model.h"
#include "canvas_widget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CanvasModel model;
    CanvasWidget w(model);
    w.resize(800, 600);
    w.show();

    return app.exec();
}
