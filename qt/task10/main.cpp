#include <QApplication>

#include "polybool_common/main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    PolyBoolUiConfig cfg;
    cfg.windowTitle = QObject::tr("Задача 10 — булевы операции для двух полигонов");
    cfg.configA.allowHoles = true;
    cfg.configA.allowExtraContours = false;
    cfg.configB.allowHoles = true;
    cfg.configB.allowExtraContours = false;
    PolyBoolMainWindow w(cfg);
    w.resize(960, 720);
    w.show();
    return app.exec();
}
