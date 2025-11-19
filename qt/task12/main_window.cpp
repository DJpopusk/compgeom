#include "main_window.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

Task12MainWindow::Task12MainWindow(QWidget* parent)
    : QWidget(parent) {
    model_.reset();
    canvas_ = new Task12Canvas(model_, this);
    connect(canvas_, &Task12Canvas::modelChanged, this, &Task12MainWindow::refresh);

    btnClose_ = new QPushButton(tr("Закрыть контур"), this);
    btnNewContour_ = new QPushButton(tr("Новый контур"), this);
    btnNewHole_ = new QPushButton(tr("Новая дырка"), this);
    btnFinalize_ = new QPushButton(tr("Завершить фигуру"), this);
    btnReset_ = new QPushButton(tr("Сброс"), this);
    cbLive_ = new QCheckBox(tr("Онлайн"), this);
    cbLive_->setChecked(true);

    lblPhase_ = new QLabel(this);
    lblStatus_ = new QLabel(this);

    connect(btnClose_, &QPushButton::clicked, this, &Task12MainWindow::onCloseContour);
    connect(btnNewContour_, &QPushButton::clicked, this, &Task12MainWindow::onNewContour);
    connect(btnNewHole_, &QPushButton::clicked, this, &Task12MainWindow::onNewHole);
    connect(btnFinalize_, &QPushButton::clicked, this, &Task12MainWindow::onFinalize);
    connect(btnReset_, &QPushButton::clicked, this, &Task12MainWindow::onReset);
    connect(cbLive_, &QCheckBox::toggled, this, &Task12MainWindow::onLiveToggled);

    auto controls = new QHBoxLayout();
    controls->addWidget(btnClose_);
    controls->addWidget(btnNewContour_);
    controls->addWidget(btnNewHole_);
    controls->addWidget(btnFinalize_);
    controls->addWidget(btnReset_);
    controls->addStretch(1);
    controls->addWidget(cbLive_);

    auto layout = new QVBoxLayout(this);
    layout->addLayout(controls);
    layout->addWidget(lblPhase_);
    layout->addWidget(lblStatus_);
    layout->addWidget(canvas_, 1);
    setLayout(layout);
    setWindowTitle(tr("Задача 12 — произвольный полигон и точка"));

    refresh();
}

void Task12MainWindow::onCloseContour() {
    if (model_.closeContour()) refresh();
}

void Task12MainWindow::onNewContour() {
    if (model_.startContour(false)) refresh();
}

void Task12MainWindow::onNewHole() {
    if (model_.startContour(true)) refresh();
}

void Task12MainWindow::onFinalize() {
    if (model_.finalizeShape()) refresh();
}

void Task12MainWindow::onReset() {
    model_.reset();
    refresh();
}

void Task12MainWindow::onLiveToggled(bool on) {
    canvas_->setLive(on);
}

QString Task12MainWindow::statusText() const {
    if (model_.phase() == Task12Phase::EditingContour) return tr("Добавляйте точки и закройте контур.");
    if (model_.phase() == Task12Phase::Idle) return tr("Начните новый контур или завершите фигуру.");
    if (!model_.hasQueryPoint()) return tr("Поставьте точку.");
    const auto& cls = model_.classification();
    QString base;
    switch (cls.region) {
        case task12::Region::Inside: base = tr("Внутри"); break;
        case task12::Region::Outside: base = tr("Снаружи"); break;
        case task12::Region::Boundary: base = tr("На границе"); break;
        case task12::Region::NearBoundary: base = tr("Близко к границе"); break;
    }
    return base + QString(" | dist ≈ %1 | delta ≈ %2")
                       .arg(static_cast<double>(cls.distance), 0, 'f', 4)
                       .arg(static_cast<double>(cls.delta), 0, 'f', 4);
}

void Task12MainWindow::refresh() {
    canvas_->update();
    QString phase;
    switch (model_.phase()) {
        case Task12Phase::EditingContour: phase = tr("Редактирование контура"); break;
        case Task12Phase::Idle: phase = tr("Контур завершён"); break;
        case Task12Phase::Ready: phase = tr("Фигура готова"); break;
        case Task12Phase::Query: phase = tr("Проверка точки"); break;
    }
    lblPhase_->setText(phase);
    lblStatus_->setText(statusText());
}

