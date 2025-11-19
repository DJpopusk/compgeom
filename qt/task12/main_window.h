#pragma once

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "canvas_model.h"
#include "canvas_widget.h"

class Task12MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit Task12MainWindow(QWidget* parent = nullptr);

private slots:
    void onCloseContour();
    void onNewContour();
    void onNewHole();
    void onFinalize();
    void onReset();
    void onLiveToggled(bool on);
    void refresh();

private:
    QString statusText() const;

    Task12Model model_;
    Task12Canvas* canvas_ = nullptr;
    QPushButton* btnClose_ = nullptr;
    QPushButton* btnNewContour_ = nullptr;
    QPushButton* btnNewHole_ = nullptr;
    QPushButton* btnFinalize_ = nullptr;
    QPushButton* btnReset_ = nullptr;
    QCheckBox* cbLive_ = nullptr;
    QLabel* lblPhase_ = nullptr;
    QLabel* lblStatus_ = nullptr;
};

