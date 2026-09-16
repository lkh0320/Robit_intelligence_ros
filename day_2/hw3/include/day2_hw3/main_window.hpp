#ifndef day2_hw3_MAIN_WINDOW_H
#define day2_hw3_MAIN_WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
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

private Q_SLOTS:
  void onIntReceived(int value);
  void onBoolReceived(bool value);
  void onCharReceived(QString value);
  void onServiceResponded(QString message);
  void onCheckTimer();

private:
  struct TopicMonitor
  {
    QLabel* value_label;
    QLabel* count_label;
    QLabel* state_label;
    qint64 last_time_ms;
    int count;
  };

  Ui::MainWindowDesign* ui;
  QTimer* check_timer_;
  TopicMonitor monitors_[3];

  void updateTopic(int index, const QString& value);
  void closeEvent(QCloseEvent* event);
};

#endif
