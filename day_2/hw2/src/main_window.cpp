#include "../include/day2_hw2/main_window.hpp"
#include <cmath>

static const int TIMER_MS = 100;
static const double LINEAR_SPEED = 2.0;
static const double ANGULAR_SPEED = 2.0;

//UI 생성, QNode 시작, 시그널 슬롯과 타이머 연결
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowDesign)
{
  ui->setupUi(this);

  QIcon icon("://ros-icon.png");
  this->setWindowIcon(icon);

  qnode = new QNode();

  QObject::connect(qnode, SIGNAL(rosShutDown()), this, SLOT(close()));

  connect(qnode, &QNode::poseReceived, this, &MainWindow::onPoseReceived);

  connect(ui->btnForward, &QPushButton::pressed, this, [this]() { linear_ = LINEAR_SPEED; });
  connect(ui->btnForward, &QPushButton::released, this, [this]() { linear_ = 0.0; });
  connect(ui->btnBackward, &QPushButton::pressed, this, [this]() { linear_ = -LINEAR_SPEED; });
  connect(ui->btnBackward, &QPushButton::released, this, [this]() { linear_ = 0.0; });
  connect(ui->btnLeft, &QPushButton::pressed, this, [this]() { angular_ = ANGULAR_SPEED; });
  connect(ui->btnLeft, &QPushButton::released, this, [this]() { angular_ = 0.0; });
  connect(ui->btnRight, &QPushButton::pressed, this, [this]() { angular_ = -ANGULAR_SPEED; });
  connect(ui->btnRight, &QPushButton::released, this, [this]() { angular_ = 0.0; });

  connect(ui->btnRecordStart, &QPushButton::clicked, this, &MainWindow::onRecordStartClicked);
  connect(ui->btnRecordStop, &QPushButton::clicked, this, &MainWindow::onRecordStopClicked);
  connect(ui->btnReplay, &QPushButton::clicked, this, &MainWindow::onReplayClicked);

  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &MainWindow::onTimer);
  timer_->start(TIMER_MS);

  setFocusPolicy(Qt::StrongFocus);
  setFocus();

  updateButtons();
  updateStatus();
}

void MainWindow::onTimer() //100ms마다 기록 또는 재생 진행, cmd_vel 발행, 상태 출력
{
  if (mode_ == RECORDING)
  {
    recordPose();
  }
  else if (mode_ == REPLAYING)
  {
    replayNextPose();
  }

  if (mode_ != REPLAYING)
  {
    qnode->publishCmdVel(linear_, angular_);
  }

  updateStatus();
}

void MainWindow::onPoseReceived(double x, double y, double theta) //받은 최신 pose 저장
{
  current_pose_.x = x;
  current_pose_.y = y;
  current_pose_.theta = theta;
  has_pose_ = true;
}

void MainWindow::onRecordStartClicked() //기록 시작
{
  path_.clear();
  total_distance_ = 0.0;
  elapsed_timer_.start();
  mode_ = RECORDING;
  updateButtons();
  setFocus();
}

void MainWindow::onRecordStopClicked() //기록 종료
{
  last_seconds_ = elapsed_timer_.elapsed() / 1000.0;
  mode_ = IDLE;
  updateButtons();
  setFocus();
}

void MainWindow::onReplayClicked() //재생 시작 (펜 끄고 시작점으로 이동)
{
  if (path_.isEmpty())
  {
    return;
  }

  linear_ = 0.0;
  angular_ = 0.0;
  qnode->publishCmdVel(0.0, 0.0);

  qnode->setPen(255, 0, 0, 3, true);

  replay_index_ = 0;
  total_distance_ = 0.0;
  elapsed_timer_.start();
  mode_ = REPLAYING;
  updateButtons();
}

void MainWindow::recordPose() //현재 pose 기록, 이동 거리 누적
{
  if (!has_pose_)
  {
    return;
  }

  if (!path_.isEmpty())
  {
    total_distance_ += distance(path_.last(), current_pose_);
  }
  path_.append(current_pose_);
}

void MainWindow::replayNextPose() //기록된 다음 위치로 teleport
{
  if (replay_index_ >= path_.size())
  {
    finishReplay();
    return;
  }

  if (replay_index_ == 1)
  {
    qnode->setPen(255, 0, 0, 3, false);
  }

  if (replay_index_ > 0)
  {
    total_distance_ += distance(path_[replay_index_ - 1], path_[replay_index_]);
  }

  const PoseData& p = path_[replay_index_];
  qnode->teleport(p.x, p.y, p.theta);
  replay_index_++;
}

void MainWindow::finishReplay() //재생 종료, 펜 색상 원래대로
{
  last_seconds_ = elapsed_timer_.elapsed() / 1000.0;
  qnode->setPen(179, 184, 255, 3, false);
  mode_ = IDLE;
  updateButtons();
  setFocus();
}

double MainWindow::distance(const PoseData& a, const PoseData& b) //두 위치 사이 거리 계산
{
  double dx = b.x - a.x;
  double dy = b.y - a.y;
  return std::sqrt(dx * dx + dy * dy);
}

void MainWindow::updateButtons() //상태에 맞게 버튼 활성화
{
  ui->btnRecordStart->setEnabled(mode_ == IDLE);
  ui->btnRecordStop->setEnabled(mode_ == RECORDING);
  ui->btnReplay->setEnabled(mode_ == IDLE && !path_.isEmpty());
  ui->groupDrive->setEnabled(mode_ != REPLAYING);
}

void MainWindow::updateStatus() //상태, 점 개수, 이동 거리, 소요 시간, 현재 위치 출력
{
  QString state;
  QString points;
  double seconds = last_seconds_;

  if (mode_ == IDLE)
  {
    state = "대기";
    points = QString::number(path_.size());
  }
  else if (mode_ == RECORDING)
  {
    state = "기록 중";
    points = QString::number(path_.size());
    seconds = elapsed_timer_.elapsed() / 1000.0;
  }
  else
  {
    state = "재생 중";
    points = QString("%1 / %2").arg(replay_index_).arg(path_.size());
    seconds = elapsed_timer_.elapsed() / 1000.0;
  }

  ui->labelState->setText("상태 : " + state);
  ui->labelPoints->setText("기록된 점 : " + points);
  ui->labelDistance->setText(QString("총 이동 거리 : %1").arg(total_distance_, 0, 'f', 2));
  ui->labelTime->setText(QString("소요 시간 : %1 초").arg(seconds, 0, 'f', 1));
  ui->labelPose->setText(QString("현재 위치 : x %1  y %2  theta %3")
                             .arg(current_pose_.x, 0, 'f', 2)
                             .arg(current_pose_.y, 0, 'f', 2)
                             .arg(current_pose_.theta, 0, 'f', 2));
}

void MainWindow::keyPressEvent(QKeyEvent* event) //WASD 누르면 해당 방향 속도 설정
{
  if (event->isAutoRepeat() || mode_ == REPLAYING)
  {
    return;
  }

  switch (event->key())
  {
    case Qt::Key_W:
      linear_ = LINEAR_SPEED;
      break;
    case Qt::Key_S:
      linear_ = -LINEAR_SPEED;
      break;
    case Qt::Key_A:
      angular_ = ANGULAR_SPEED;
      break;
    case Qt::Key_D:
      angular_ = -ANGULAR_SPEED;
      break;
    default:
      QMainWindow::keyPressEvent(event);
      break;
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) //WASD 떼면 해당 방향 속도 0
{
  if (event->isAutoRepeat())
  {
    return;
  }

  switch (event->key())
  {
    case Qt::Key_W:
    case Qt::Key_S:
      linear_ = 0.0;
      break;
    case Qt::Key_A:
    case Qt::Key_D:
      angular_ = 0.0;
      break;
    default:
      QMainWindow::keyReleaseEvent(event);
      break;
  }
}

void MainWindow::closeEvent(QCloseEvent* event) //창 닫을 때 ROS 종료, 스레드 정리
{
  timer_->stop();
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
