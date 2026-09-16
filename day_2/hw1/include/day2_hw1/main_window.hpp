#ifndef day2_hw1_MAIN_WINDOW_H
#define day2_hw1_MAIN_WINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
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
  void onControlTimer();
  void onCmdVelReceived(double linear, double angular);
  void onTriangleClicked();
  void onSquareClicked();
  void onCircleClicked();
  void onPenChanged();
  void onClearClicked();

private:
  struct Step
  {
    double linear;
    double angular;
    int ticks;
  };

  Ui::MainWindowDesign* ui;
  QTimer* control_timer_;

  double linear_ = 0.0;
  double angular_ = 0.0;

  QVector<Step> steps_;
  bool shape_running_ = false;
  QString shape_name_;

  QVector<Step> makePolygon(int sides, double side_length);
  QVector<Step> makeCircle(double radius);
  void startShape(const QVector<Step>& steps, const QString& name);
  void cancelShape();
  void closeEvent(QCloseEvent* event);
};

#endif
