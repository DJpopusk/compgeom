#pragma once
#include <QWidget>

class QPushButton;
class QCheckBox;
class QRadioButton;
class QLabel;
class CanvasWidget;
class CanvasModel;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onStartB();
    void onFinish();
    void onLiveToggled(bool on);
    void onStatusFromCanvas(const QString& s);
    void onOpChanged();

private:
    CanvasModel*  model_ = nullptr;
    CanvasWidget* canvas_ = nullptr;

    QPushButton *btnStartB_, *btnFinish_;
    QCheckBox   *cbLive_;
    QRadioButton *rbInter_, *rbUnion_, *rbDiff_;
    QLabel *lblStatus_;
};
