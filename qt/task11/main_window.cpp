#include "main_window.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

Task11MainWindow::Task11MainWindow(QWidget* parent)
    : QWidget(parent) {
    canvas_ = new Task11Canvas(model_, this);
    connect(canvas_, &Task11Canvas::modelChanged, this, &Task11MainWindow::refresh);

    btnFinish_ = new QPushButton(tr("Завершить оболочку"), this);
    btnReset_ = new QPushButton(tr("Сброс"), this);
    cbLive_ = new QCheckBox(tr("Онлайн"), this);
    cbLive_->setChecked(true);

    lblStatus_ = new QLabel(this);
    lblPhase_ = new QLabel(this);

    connect(btnFinish_, &QPushButton::clicked, this, &Task11MainWindow::onFinish);
    connect(btnReset_, &QPushButton::clicked, this, &Task11MainWindow::onReset);
    connect(cbLive_, &QCheckBox::toggled, this, &Task11MainWindow::onLiveToggled);

    auto controls = new QHBoxLayout();
    controls->addWidget(btnFinish_);
    controls->addWidget(btnReset_);
    controls->addStretch(1);
    controls->addWidget(cbLive_);

    auto layout = new QVBoxLayout(this);
    layout->addLayout(controls);
    layout->addWidget(lblPhase_);
    layout->addWidget(lblStatus_);
    layout->addWidget(canvas_, 1);
    setLayout(layout);
    setWindowTitle(tr("Задача 11 — точка и выпуклая оболочка"));

    refresh();
}

void Task11MainWindow::onFinish() {
    if (model_.finishHull()) refresh();
}

void Task11MainWindow::onReset() {
    model_.reset();
    refresh();
}

void Task11MainWindow::onLiveToggled(bool on) {
    canvas_->setLive(on);
}

QString Task11MainWindow::classificationText() const {
    if (!model_.hasHull()) return tr("Добавьте точки и завершите оболочку.");
    if (!model_.hasQueryPoint()) return tr("Поставьте точку для проверки.");
    const auto& cls = model_.classification();
    QString base;
    switch (cls.region) {
        case task11::Region::Inside: base = tr("Внутри"); break;
        case task11::Region::Outside: base = tr("Снаружи"); break;
        case task11::Region::Boundary: base = tr("На границе"); break;
        case task11::Region::NearBoundary: base = tr("Близко к границе"); break;
    }
    return base + QString(" | dist ≈ %1 | delta ≈ %2")
                       .arg(static_cast<double>(cls.distance), 0, 'f', 4)
                       .arg(static_cast<double>(cls.delta), 0, 'f', 4);
}

void Task11MainWindow::refresh() {
    canvas_->update();
    QString phase;
    switch (model_.phase()) {
        case Task11Phase::Collecting: phase = tr("Сбор точек"); break;
        case Task11Phase::HullReady: phase = tr("Оболочка построена"); break;
        case Task11Phase::Query: phase = tr("Точка поставлена"); break;
    }
    lblPhase_->setText(phase);
    lblStatus_->setText(classificationText());
}

