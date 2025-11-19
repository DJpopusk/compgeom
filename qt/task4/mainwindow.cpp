#include "mainwindow.h"
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

    btnCompute_ = new QPushButton(tr("Построить оболочку"), this);
    cbLive_     = new QCheckBox(tr("Онлайн"), this);
    lblStatus_  = new QLabel(tr("points: 0   hull verts: 0"), this);

    auto h = new QHBoxLayout();
    h->addWidget(btnCompute_);
    h->addWidget(cbLive_);
    h->addStretch(1);
    h->addWidget(lblStatus_);

    auto v = new QVBoxLayout(this);
    v->addLayout(h);
    v->addWidget(canvas_, 1);

    setLayout(v);
    setWindowTitle(tr("Выпуклая оболочка — кнопка / онлайн"));

    connect(btnCompute_, &QPushButton::clicked, this, &MainWindow::onComputeClicked);
    connect(cbLive_, &QCheckBox::toggled, this, &MainWindow::onLiveToggled);
    connect(canvas_, &CanvasWidget::hullStatus, this, &MainWindow::onHullStatus);
}

void MainWindow::onComputeClicked() {
    canvas_->computeOnce();
}

void MainWindow::onLiveToggled(bool on) {
    canvas_->setLive(on);
    btnCompute_->setEnabled(!on);
}

void MainWindow::onHullStatus(int points, int hullVerts) {
    lblStatus_->setText(QString("points: %1   hull verts: %2").arg(points).arg(hullVerts));
}
