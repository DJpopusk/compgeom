#pragma once

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "canvas_model.h"
#include "canvas_widget.h"

class Task11MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit Task11MainWindow(QWidget* parent = nullptr);

private slots:
    void onFinish();
    void onReset();
    void onLiveToggled(bool on);
    void refresh();

private:
    QString classificationText() const;

    Task11Model model_;
    Task11Canvas* canvas_ = nullptr;
    QPushButton* btnFinish_ = nullptr;
    QPushButton* btnReset_ = nullptr;
    QCheckBox* cbLive_ = nullptr;
    QLabel* lblStatus_ = nullptr;
    QLabel* lblPhase_ = nullptr;
};
