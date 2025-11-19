#include "main_window.h"
#include "canvas_widget.h"
#include "canvas_model.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include <string>

static QLineEdit* mkEdit(QWidget* parent, const QString& ph) {
    auto* e = new QLineEdit(parent);
    e->setPlaceholderText(ph);
    e->setClearButtonEnabled(true);
    e->setMinimumWidth(130);
    return e;
}

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    model_ = new CanvasModel();
    canvas_ = new CanvasWidget(*model_, this);

    ax_ = mkEdit(this, "Ax"); ay_ = mkEdit(this, "Ay");
    bx_ = mkEdit(this, "Bx"); by_ = mkEdit(this, "By");
    px_ = mkEdit(this, "Px"); py_ = mkEdit(this, "Py");
    eps_ = mkEdit(this, "epsilon");

    btnApply_ = new QPushButton(tr("Применить из полей"), this);
    btnReset_ = new QPushButton(tr("Сброс вида"), this);
    lblRel_   = new QLabel(tr("relation: —"), this);

    // Верхняя панель ввода
    auto grid = new QGridLayout();
    grid->addWidget(new QLabel("A:"), 0,0); grid->addWidget(ax_,0,1); grid->addWidget(ay_,0,2);
    grid->addWidget(new QLabel("B:"), 1,0); grid->addWidget(bx_,1,1); grid->addWidget(by_,1,2);
    grid->addWidget(new QLabel("P:"), 2,0); grid->addWidget(px_,2,1); grid->addWidget(py_,2,2);
    grid->addWidget(new QLabel("ε:"), 0,3); grid->addWidget(eps_,0,4);
    grid->addWidget(btnApply_, 1,3,1,2);
    grid->addWidget(btnReset_, 2,3,1,2);
    grid->addWidget(lblRel_,   0,5,3,1);

    auto v = new QVBoxLayout(this);
    v->addLayout(grid);
    v->addWidget(canvas_, 1);
    setLayout(v);
    setWindowTitle(tr("Point vs Segment — ввод координат и клики"));

    connect(btnApply_, &QPushButton::clicked, this, &MainWindow::applyFromFields);
    connect(btnReset_, &QPushButton::clicked, this, &MainWindow::resetView);
    connect(canvas_, &CanvasWidget::relationChanged, this, &MainWindow::onRelation);

    // Начальные значения
    ax_->setText("1");
    ay_->setText("1");
    bx_->setText("2");
    by_->setText("2");
    px_->setText("1");
    py_->setText("1.000000000000000000000000000000000000000000000000000000000000000000001");
    eps_->setText("0"); // начнём без толеранса
}

bool MainWindow::parseLD(const QString& s, long double& out, QString* err) const {
    try {
        // std::stold — парсит long double и поддерживает длинные десятичные строки
        std::string tmp = s.trimmed().toStdString();
        if (tmp.empty()) { if(err) *err="empty"; return false; }
        size_t pos = 0;
        out = std::stold(tmp, &pos);
        if (pos != tmp.size()) { if(err) *err="trailing chars"; return false; }
        return true;
    } catch (...) {
        if (err) *err = "bad number";
        return false;
    }
}

void MainWindow::applyFromFields() {
    long double ax, ay, bx, by, px, py, e;
    QString err;

    auto bad = [&](QLineEdit* w){ w->setStyleSheet("QLineEdit { background-color: #ffdddd; }"); };
    auto ok  = [&](QLineEdit* w){ w->setStyleSheet(""); };

    bool allok = true;
    if (!parseLD(ax_->text(), ax, &err)) { bad(ax_); allok=false; } else ok(ax_);
    if (!parseLD(ay_->text(), ay, &err)) { bad(ay_); allok=false; } else ok(ay_);
    if (!parseLD(bx_->text(), bx, &err)) { bad(bx_); allok=false; } else ok(bx_);
    if (!parseLD(by_->text(), by, &err)) { bad(by_); allok=false; } else ok(by_);
    if (!parseLD(px_->text(), px, &err)) { bad(px_); allok=false; } else ok(px_);
    if (!parseLD(py_->text(), py, &err)) { bad(py_); allok=false; } else ok(py_);
    if (!parseLD(eps_->text(), e, &err)) { bad(eps_); allok=false; } else ok(eps_);

    if (!allok) return;

    model_->setA({ax, ay});
    model_->setB({bx, by});
    model_->setP({px, py});
    model_->setEps(e);

    onRelation(model_->relation());
    canvas_->update();
}

void MainWindow::resetView() {
    canvas_->resetView();
}

void MainWindow::onRelation(int r) {
    QString s = "relation: ";
    if      (r ==  1) s += "LEFT (1)";
    else if (r == -1) s += "RIGHT (-1)";
    else if (r ==  0) s += "ON (0)";
    else              s += "— (недостаточно данных)";
    lblRel_->setText(s);
}
