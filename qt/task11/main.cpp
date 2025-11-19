#include <QApplication>

#include "polybool_common/main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    PolyBoolUiConfig cfg;
    cfg.windowTitle = QObject::tr("Задача 11 — произвольные полигоны");
    cfg.configA.allowHoles = false;
    cfg.configA.allowExtraContours = true;
    cfg.configB.allowHoles = false;
    cfg.configB.allowExtraContours = true;
    PolyBoolMainWindow w(cfg);
    w.resize(960, 720);
    w.show();
    return app.exec();
}

