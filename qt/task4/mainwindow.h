#pragma once
#include <QWidget>

class QPushButton;
class QCheckBox;
class QLabel;
class CanvasWidget;
class CanvasModel;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onComputeClicked();
    void onLiveToggled(bool on);
    void onHullStatus(int points, int hullVerts);

private:
    CanvasModel*  model_ = nullptr;
    CanvasWidget* canvas_ = nullptr;

    QPushButton*  btnCompute_ = nullptr;
    QCheckBox*    cbLive_ = nullptr;
    QLabel*       lblStatus_ = nullptr;
};
