#pragma once

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>

#include "canvas_model.h"
#include "canvas_widget.h"

struct PolyBoolUiConfig {
    QString windowTitle;
    PolyBoolConfig configA;
    PolyBoolConfig configB;
    bool enableLive = true;
};

class PolyBoolMainWindow : public QWidget {
    Q_OBJECT
public:
    PolyBoolMainWindow(const PolyBoolUiConfig& cfg, QWidget* parent = nullptr);

private slots:
    void onCloseContour();
    void onStartContour();
    void onStartHole();
    void onStartSecond();
    void onFinishSecond();
    void onReset();
    void onModeChanged();
    void onLiveToggled(bool on);
    void onCanvasChanged();

private:
    void refresh();
    QString phaseText() const;
    QString modeText() const;
    int countClosed(const std::vector<PolyBoolModel::Outline>& outlines) const;

    PolyBoolUiConfig config_;
    PolyBoolModel model_;
    PolyBoolCanvas* canvas_ = nullptr;

    QPushButton* btnClose_ = nullptr;
    QPushButton* btnNewContour_ = nullptr;
    QPushButton* btnNewHole_ = nullptr;
    QPushButton* btnSecond_ = nullptr;
    QPushButton* btnFinish_ = nullptr;
    QPushButton* btnReset_ = nullptr;
    QCheckBox* cbLive_ = nullptr;
    QRadioButton* rbInter_ = nullptr;
    QRadioButton* rbUnion_ = nullptr;
    QRadioButton* rbDiff_ = nullptr;
    QLabel* lblPhase_ = nullptr;
    QLabel* lblInfo_ = nullptr;
};

