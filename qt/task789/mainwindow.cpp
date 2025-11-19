#include "mainwindow.h"
#include "canvas_widget.h"
#include "canvas_model.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    model_  = new CanvasModel();
    canvas_ = new CanvasWidget(*model_, this);

    btnStartB_ = new QPushButton(tr("Создать второй"), this);
    btnFinish_ = new QPushButton(tr("Завершить"), this);
    cbLive_    = new QCheckBox(tr("Онлайн"), this);

    rbInter_   = new QRadioButton(tr("Пересечение"), this);
    rbUnion_   = new QRadioButton(tr("Объединение"), this);
    rbDiff_    = new QRadioButton(tr("Разность A\\B"), this);
    rbInter_->setChecked(true);

    lblStatus_ = new QLabel(tr("Редактирование A"), this);

    auto ops = new QHBoxLayout();
    ops->addWidget(rbInter_);
    ops->addWidget(rbUnion_);
    ops->addWidget(rbDiff_);

    auto top = new QHBoxLayout();
    top->addWidget(btnStartB_);
    top->addWidget(btnFinish_);
    top->addSpacing(16);
    top->addWidget(cbLive_);
    top->addSpacing(24);
    top->addLayout(ops);
    top->addStretch(1);
    top->addWidget(lblStatus_);

    auto root = new QVBoxLayout(this);
    root->addLayout(top);
    root->addWidget(canvas_, 1);
    setLayout(root);
    setWindowTitle(tr("Два выпуклых многоугольника: булевы операции"));

    connect(btnStartB_, &QPushButton::clicked, this, &MainWindow::onStartB);
    connect(btnFinish_, &QPushButton::clicked, this, &MainWindow::onFinish);
    connect(cbLive_,    &QCheckBox::toggled,   this, &MainWindow::onLiveToggled);
    connect(canvas_,    &CanvasWidget::statusText, this, &MainWindow::onStatusFromCanvas);
    connect(rbInter_,   &QRadioButton::toggled, this, &MainWindow::onOpChanged);
    connect(rbUnion_,   &QRadioButton::toggled, this, &MainWindow::onOpChanged);
    connect(rbDiff_,    &QRadioButton::toggled, this, &MainWindow::onOpChanged);

    onOpChanged(); 
}

void MainWindow::onStartB() { canvas_->actionStartB(); }
void MainWindow::onFinish() { canvas_->actionFinish(); }

void MainWindow::onLiveToggled(bool on) { canvas_->setLive(on); }

void MainWindow::onStatusFromCanvas(const QString& s) { lblStatus_->setText(s); }

void MainWindow::onOpChanged() {
    if (rbInter_->isChecked())      model_->setOpMode(OpMode::Intersection);
    else if (rbUnion_->isChecked()) model_->setOpMode(OpMode::Union);
    else                            model_->setOpMode(OpMode::DiffAminusB);
}
