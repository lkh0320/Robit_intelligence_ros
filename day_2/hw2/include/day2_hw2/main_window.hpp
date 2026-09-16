#ifndef day2_hw2_MAIN_WINDOW_H
#define day2_hw2_MAIN_WINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include "QIcon"
#include "qnode.hpp"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  QNode* qnode;

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;

private Q_SLOTS:
  void onTimer();
  void onPoseReceived(double x, double y, double theta);
  void onRecordStartClicked();
  void onRecordStopClicked();
  void onReplayClicked();

private:
  enum Mode
  {
    IDLE,
    RECORDING,
    REPLAYING
  };

  struct PoseData
  {
    double x;
    double y;
    double theta;
  };

  Ui::MainWindowDesign* ui;
  QTimer* timer_;
  QElapsedTimer elapsed_timer_;

  Mode mode_ = IDLE;
  double linear_ = 0.0;
  double angular_ = 0.0;

  PoseData current_pose_ = { 0.0, 0.0, 0.0 };
  bool has_pose_ = false;

  QVector<PoseData> path_;
  int replay_index_ = 0;
  double total_distance_ = 0.0;
  double last_seconds_ = 0.0;

  void recordPose();
  void replayNextPose();
  void finishReplay();
  double distance(const PoseData& a, const PoseData& b);
  void updateButtons();
  void updateStatus();
  void closeEvent(QCloseEvent* event);
};

#endif
