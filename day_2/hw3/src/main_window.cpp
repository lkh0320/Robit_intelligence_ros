#include "../include/day2_hw3/main_window.hpp"
#include <QDateTime>

static const int CHECK_MS = 200;
static const int TIMEOUT_MS = 2000;

//UI 생성, QNode 시작, 시그널 슬롯과 타이머 연결
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowDesign)
{
  ui->setupUi(this);

  QIcon icon("://ros-icon.png");
  this->setWindowIcon(icon);

  monitors_[0] = { ui->labelIntValue, ui->labelIntCount, ui->labelIntState, -1, 0 };
  monitors_[1] = { ui->labelBoolValue, ui->labelBoolCount, ui->labelBoolState, -1, 0 };
  monitors_[2] = { ui->labelCharValue, ui->labelCharCount, ui->labelCharState, -1, 0 };

  qnode = new QNode();

  QObject::connect(qnode, SIGNAL(rosShutDown()), this, SLOT(close()));

  connect(qnode, &QNode::intReceived, this, &MainWindow::onIntReceived);
  connect(qnode, &QNode::boolReceived, this, &MainWindow::onBoolReceived);
  connect(qnode, &QNode::charReceived, this, &MainWindow::onCharReceived);
  connect(qnode, &QNode::serviceResponded, this, &MainWindow::onServiceResponded);

  connect(ui->btnRestartInt, &QPushButton::clicked, this, [this]() { qnode->requestRestart(0); });
  connect(ui->btnRestartBool, &QPushButton::clicked, this, [this]() { qnode->requestRestart(1); });
  connect(ui->btnRestartChar, &QPushButton::clicked, this, [this]() { qnode->requestRestart(2); });

  check_timer_ = new QTimer(this);
  connect(check_timer_, &QTimer::timeout, this, &MainWindow::onCheckTimer);
  check_timer_->start(CHECK_MS);

  onCheckTimer();
}

void MainWindow::updateTopic(int index, const QString& value) //토픽 값, 수신 횟수, 마지막 수신 시각 갱신
{
  TopicMonitor& m = monitors_[index];
  m.count++;
  m.last_time_ms = QDateTime::currentMSecsSinceEpoch();
  m.value_label->setText(value);
  m.count_label->setText(QString::number(m.count));
}

void MainWindow::onIntReceived(int value) //받은 int 값 출력
{
  updateTopic(0, QString::number(value));
}

void MainWindow::onBoolReceived(bool value) //받은 bool 값 출력
{
  updateTopic(1, value ? "true" : "false");
}

void MainWindow::onCharReceived(QString value) //받은 char 값 출력
{
  updateTopic(2, "'" + value + "'");
}

void MainWindow::onServiceResponded(QString message) //서비스 응답 출력
{
  ui->labelService->setText("서비스 응답 : " + message);
}

void MainWindow::onCheckTimer() //2초 이상 메시지가 없으면 응답 없음 표시
{
  qint64 now = QDateTime::currentMSecsSinceEpoch();

  for (int i = 0; i < 3; i++)
  {
    TopicMonitor& m = monitors_[i];

    if (m.last_time_ms < 0)
    {
      m.state_label->setText("대기 중");
      m.state_label->setStyleSheet("color: gray;");
    }
    else if (now - m.last_time_ms > TIMEOUT_MS)
    {
      m.state_label->setText("응답 없음");
      m.state_label->setStyleSheet("color: red; font-weight: bold;");
    }
    else
    {
      m.state_label->setText("연결됨");
      m.state_label->setStyleSheet("color: green; font-weight: bold;");
    }
  }
}

void MainWindow::closeEvent(QCloseEvent* event) //창 닫을 때 ROS 종료, 스레드 정리
{
  check_timer_->stop();
  if (rclcpp::ok())
  {
    rclcpp::shutdown();
  }
  qnode->wait();
  QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow() //메모리 해제
{
  delete qnode;
  delete ui;
}
