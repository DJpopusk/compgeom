#include <QApplication>

#include "polybool_common/main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    PolyBoolUiConfig cfg;
    cfg.windowTitle = QObject::tr("Задача 12 — полигоны с дырками и частями");
    cfg.configA.allowHoles = true;
    cfg.configA.allowExtraContours = true;
    cfg.configB.allowHoles = true;
    cfg.configB.allowExtraContours = true;
    PolyBoolMainWindow w(cfg);
    w.resize(960, 720);
    w.show();
    return app.exec();
}

