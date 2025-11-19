#pragma once
#include <QWidget>

class QLineEdit;
class QPushButton;
class QLabel;
class CanvasWidget;
class CanvasModel;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void applyFromFields();
    void resetView();
    void onRelation(int r);

private:
    bool parseLD(const QString& s, long double& out, QString* err = nullptr) const;

private:
    CanvasModel*  model_ = nullptr;
    CanvasWidget* canvas_ = nullptr;

    QLineEdit *ax_, *ay_, *bx_, *by_, *px_, *py_, *eps_;
    QPushButton *btnApply_, *btnReset_;
    QLabel *lblRel_;
};
