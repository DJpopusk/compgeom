#include "main_window.h"
#include "canvas_widget.h"
#include "canvas_model.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    model_ = new CanvasModel();
    canvas_ = new CanvasWidget(*model_, this);

    btnCompute_ = new QPushButton(tr("Вычислить пересечение"), this);
    cbLive_     = new QCheckBox(tr("Онлайн (двигать точки)"), this);
    lblStatus_  = new QLabel(tr("Готовность: поставьте 4 точки"), this);

    auto h = new QHBoxLayout();
    h->addWidget(btnCompute_);
    h->addWidget(cbLive_);
    h->addStretch(1);
    h->addWidget(lblStatus_);

    auto v = new QVBoxLayout(this);
    v->addLayout(h);
    v->addWidget(canvas_, 1);

    setLayout(v);
    setWindowTitle(tr("Пересечение двух отрезков — кнопка / онлайн"));

    // связи
    connect(btnCompute_, &QPushButton::clicked, this, &MainWindow::onComputeClicked);
    connect(cbLive_, &QCheckBox::toggled, this, &MainWindow::onLiveToggled);
    connect(canvas_, &CanvasWidget::intersectionAvailable, this, &MainWindow::onIntersectionStatus);
}

void MainWindow::onComputeClicked() {
    canvas_->computeOnce();
}
void MainWindow::onLiveToggled(bool on) {
    canvas_->setLive(on);
    btnCompute_->setEnabled(!on);
}

void MainWindow::onIntersectionStatus(bool ok) {
    if (ok) lblStatus_->setText(tr("Пересечение: есть"));
    else    lblStatus_->setText(tr("Пересечение: нет или не хватает точек"));
}
