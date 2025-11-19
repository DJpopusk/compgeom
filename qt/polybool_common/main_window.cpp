#include "main_window.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

PolyBoolMainWindow::PolyBoolMainWindow(const PolyBoolUiConfig& cfg, QWidget* parent)
    : QWidget(parent), config_(cfg), model_(cfg.configA, cfg.configB) {
    canvas_ = new PolyBoolCanvas(model_, this);
    connect(canvas_, &PolyBoolCanvas::canvasChanged, this, &PolyBoolMainWindow::onCanvasChanged);

    btnClose_ = new QPushButton(tr("Закрыть контур"), this);
    btnNewContour_ = new QPushButton(tr("Новый контур"), this);
    btnNewHole_ = new QPushButton(tr("Новая дырка"), this);
    btnSecond_ = new QPushButton(tr("Создать второй"), this);
    btnFinish_ = new QPushButton(tr("Завершить"), this);
    btnReset_ = new QPushButton(tr("Сброс"), this);
    cbLive_ = new QCheckBox(tr("Онлайн"), this);
    cbLive_->setChecked(true);

    rbInter_ = new QRadioButton(tr("Пересечение"), this);
    rbUnion_ = new QRadioButton(tr("Объединение"), this);
    rbDiff_ = new QRadioButton(tr("Разность A/B"), this);
    rbInter_->setChecked(true);

    lblPhase_ = new QLabel(this);
    lblInfo_ = new QLabel(this);

    connect(btnClose_, &QPushButton::clicked, this, &PolyBoolMainWindow::onCloseContour);
    connect(btnNewContour_, &QPushButton::clicked, this, &PolyBoolMainWindow::onStartContour);
    connect(btnNewHole_, &QPushButton::clicked, this, &PolyBoolMainWindow::onStartHole);
    connect(btnSecond_, &QPushButton::clicked, this, &PolyBoolMainWindow::onStartSecond);
    connect(btnFinish_, &QPushButton::clicked, this, &PolyBoolMainWindow::onFinishSecond);
    connect(btnReset_, &QPushButton::clicked, this, &PolyBoolMainWindow::onReset);
    connect(cbLive_, &QCheckBox::toggled, this, &PolyBoolMainWindow::onLiveToggled);
    connect(rbInter_, &QRadioButton::toggled, this, &PolyBoolMainWindow::onModeChanged);
    connect(rbUnion_, &QRadioButton::toggled, this, &PolyBoolMainWindow::onModeChanged);
    connect(rbDiff_, &QRadioButton::toggled, this, &PolyBoolMainWindow::onModeChanged);

    auto ops = new QHBoxLayout();
    ops->addWidget(rbInter_);
    ops->addWidget(rbUnion_);
    ops->addWidget(rbDiff_);

    auto controls = new QHBoxLayout();
    controls->addWidget(btnClose_);
    controls->addWidget(btnNewContour_);
    controls->addWidget(btnNewHole_);
    controls->addWidget(btnSecond_);
    controls->addWidget(btnFinish_);
    controls->addWidget(btnReset_);
    controls->addStretch(1);
    if (config_.enableLive) controls->addWidget(cbLive_);

    auto root = new QVBoxLayout(this);
    root->addLayout(controls);
    root->addLayout(ops);
    root->addWidget(lblPhase_);
    root->addWidget(lblInfo_);
    root->addWidget(canvas_, 1);
    setLayout(root);
    setWindowTitle(config_.windowTitle);

    refresh();
}

void PolyBoolMainWindow::onCloseContour() {
    if (model_.closeContour()) onCanvasChanged();
}

void PolyBoolMainWindow::onStartContour() {
    if (model_.startContour(false)) onCanvasChanged();
}

void PolyBoolMainWindow::onStartHole() {
    if (model_.startContour(true)) onCanvasChanged();
}

void PolyBoolMainWindow::onStartSecond() {
    if (model_.startSecondPolygon()) onCanvasChanged();
}

void PolyBoolMainWindow::onFinishSecond() {
    if (model_.finishSecondPolygon()) onCanvasChanged();
}

void PolyBoolMainWindow::onReset() {
    model_.reset();
    onCanvasChanged();
}

void PolyBoolMainWindow::onModeChanged() {
    if (rbInter_->isChecked()) model_.setMode(PolyBoolMode::Intersection);
    else if (rbUnion_->isChecked()) model_.setMode(PolyBoolMode::Union);
    else if (rbDiff_->isChecked()) model_.setMode(PolyBoolMode::Difference);
    refresh();
}

void PolyBoolMainWindow::onLiveToggled(bool on) {
    canvas_->setLive(on);
}

void PolyBoolMainWindow::onCanvasChanged() {
    refresh();
}

QString PolyBoolMainWindow::phaseText() const {
    switch (model_.phase()) {
        case PolyBoolPhase::EditingA: return tr("Полигон A: ввод");
        case PolyBoolPhase::IdleA: return tr("Полигон A: завершен");
        case PolyBoolPhase::EditingB: return tr("Полигон B: ввод");
        case PolyBoolPhase::IdleB: return tr("Полигон B: завершен");
        case PolyBoolPhase::Ready: return tr("Результат готов");
    }
    return {};
}

int PolyBoolMainWindow::countClosed(const std::vector<PolyBoolModel::Outline>& outlines) const {
    int count = 0;
    for (const auto& o : outlines) {
        if (o.closed && o.pts.size() >= 3) ++count;
    }
    return count;
}

QString PolyBoolMainWindow::modeText() const {
    switch (model_.mode()) {
        case PolyBoolMode::Intersection: return tr("Пересечение");
        case PolyBoolMode::Union: return tr("Объединение");
        case PolyBoolMode::Difference: return tr("Разность A/B");
    }
    return {};
}

void PolyBoolMainWindow::refresh() {
    canvas_->update();
    lblPhase_->setText(phaseText());
    const int closedA = countClosed(model_.pointsA());
    const int closedB = countClosed(model_.pointsB());
    lblInfo_->setText(tr("A: %1 контуров | B: %2 контуров | %3")
                          .arg(closedA)
                          .arg(closedB)
                          .arg(modeText()));
    btnNewHole_->setEnabled(model_.phase() == PolyBoolPhase::EditingA ||
                            model_.phase() == PolyBoolPhase::IdleA ||
                            model_.phase() == PolyBoolPhase::EditingB ||
                            model_.phase() == PolyBoolPhase::IdleB);
    btnNewContour_->setEnabled(btnNewHole_->isEnabled());
    btnClose_->setEnabled(model_.phase() == PolyBoolPhase::EditingA ||
                          model_.phase() == PolyBoolPhase::EditingB);
    btnSecond_->setEnabled(model_.phase() == PolyBoolPhase::IdleA && closedA > 0);
    btnFinish_->setEnabled(model_.phase() == PolyBoolPhase::IdleB && closedB > 0);
}

