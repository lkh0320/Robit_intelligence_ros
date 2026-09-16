#include "../include/day2_hw1/main_window.hpp"
#include <cmath>

static const int TIMER_MS = 100;
static const double LINEAR_SPEED = 2.0;
static const double ANGULAR_SPEED = 2.0;
static const double SHAPE_SPEED = 1.0;
static const double SHAPE_SIZE = 2.0;

static const int PEN_COLORS[6][3] = {
  { 179, 184, 255 }, { 255, 255, 255 }, { 255, 0, 0 }, { 0, 255, 0 }, { 255, 255, 0 }, { 0, 0, 0 }
};

//UI 생성, QNode 시작, 시그널 슬롯과 타이머 연결
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowDesign)
{
  ui->setupUi(this);

  QIcon icon("://ros-icon.png");
  this->setWindowIcon(icon);

  qnode = new QNode();

  QObject::connect(qnode, SIGNAL(rosShutDown()), this, SLOT(close()));

  connect(qnode, &QNode::cmdVelReceived, this, &MainWindow::onCmdVelReceived);

  connect(ui->btnForward, &QPushButton::pressed, this, [this]() {
    cancelShape();
    linear_ = LINEAR_SPEED;
  });
  connect(ui->btnForward, &QPushButton::released, this, [this]() { linear_ = 0.0; });
  connect(ui->btnBackward, &QPushButton::pressed, this, [this]() {
    cancelShape();
    linear_ = -LINEAR_SPEED;
  });
  connect(ui->btnBackward, &QPushButton::released, this, [this]() { linear_ = 0.0; });
  connect(ui->btnLeft, &QPushButton::pressed, this, [this]() {
    cancelShape();
    angular_ = ANGULAR_SPEED;
  });
  connect(ui->btnLeft, &QPushButton::released, this, [this]() { angular_ = 0.0; });
  connect(ui->btnRight, &QPushButton::pressed, this, [this]() {
    cancelShape();
    angular_ = -ANGULAR_SPEED;
  });
  connect(ui->btnRight, &QPushButton::released, this, [this]() { angular_ = 0.0; });

  connect(ui->btnTriangle, &QPushButton::clicked, this, &MainWindow::onTriangleClicked);
  connect(ui->btnSquare, &QPushButton::clicked, this, &MainWindow::onSquareClicked);
  connect(ui->btnCircle, &QPushButton::clicked, this, &MainWindow::onCircleClicked);

  connect(ui->comboColor, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onPenChanged);
  connect(ui->spinWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onPenChanged);
  connect(ui->btnClear, &QPushButton::clicked, this, &MainWindow::onClearClicked);

  control_timer_ = new QTimer(this);
  connect(control_timer_, &QTimer::timeout, this, &MainWindow::onControlTimer);
  control_timer_->start(TIMER_MS);

  setFocusPolicy(Qt::StrongFocus);
  setFocus();
}

void MainWindow::onControlTimer() //100ms마다 도형 단계 또는 키보드 속도로 cmd_vel 발행
{
  if (!steps_.isEmpty())
  {
    linear_ = steps_[0].linear;
    angular_ = steps_[0].angular;
    steps_[0].ticks--;
    if (steps_[0].ticks <= 0)
    {
      steps_.removeFirst();
    }
  }
  else if (shape_running_)
  {
    shape_running_ = false;
    linear_ = 0.0;
    angular_ = 0.0;
    ui->labelShapeState->setText(shape_name_ + " 완료");
  }

  qnode->publishCmdVel(linear_, angular_);
}

void MainWindow::onCmdVelReceived(double linear, double angular) //받은 cmd_vel 값을 라벨에 출력
{
  ui->labelLinear->setText(QString("linear.x  : %1").arg(linear, 0, 'f', 2));
  ui->labelAngular->setText(QString("angular.z : %1").arg(angular, 0, 'f', 2));
}

void MainWindow::keyPressEvent(QKeyEvent* event) //WASD 누르면 해당 방향 속도 설정
{
  if (event->isAutoRepeat())
  {
    return;
  }

  switch (event->key())
  {
    case Qt::Key_W:
      cancelShape();
      linear_ = LINEAR_SPEED;
      break;
    case Qt::Key_S:
      cancelShape();
      linear_ = -LINEAR_SPEED;
      break;
    case Qt::Key_A:
      cancelShape();
      angular_ = ANGULAR_SPEED;
      break;
    case Qt::Key_D:
      cancelShape();
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

QVector<MainWindow::Step> MainWindow::makePolygon(int sides, double side_length) //정다각형(삼각형, 사각형) 이동 단계 생성
{
  QVector<Step> steps;
  double turn = 2.0 * M_PI / sides;
  int move_ticks = qRound(side_length / SHAPE_SPEED * 1000.0 / TIMER_MS);
  int turn_ticks = 1000 / TIMER_MS;

  for (int i = 0; i < sides; i++)
  {
    steps.append({ SHAPE_SPEED, 0.0, move_ticks });
    steps.append({ 0.0, turn, turn_ticks });
  }
  return steps;
}

QVector<MainWindow::Step> MainWindow::makeCircle(double radius) //원 이동 단계 생성
{
  QVector<Step> steps;
  double angular = SHAPE_SPEED / radius;
  int ticks = qRound(2.0 * M_PI * radius / SHAPE_SPEED * 1000.0 / TIMER_MS);
  steps.append({ SHAPE_SPEED, angular, ticks });
  return steps;
}

void MainWindow::startShape(const QVector<Step>& steps, const QString& name) //도형 그리기 시작
{
  steps_ = steps;
  shape_running_ = true;
  shape_name_ = name;
  linear_ = 0.0;
  angular_ = 0.0;
  ui->labelShapeState->setText(name + " 그리는 중...");
  setFocus();
}

void MainWindow::cancelShape() //그리던 도형 취소
{
  if (!shape_running_)
  {
    return;
  }
  steps_.clear();
  shape_running_ = false;
  linear_ = 0.0;
  angular_ = 0.0;
  ui->labelShapeState->setText(shape_name_ + " 취소");
}

void MainWindow::onTriangleClicked() //삼각형 그리기
{
  startShape(makePolygon(3, SHAPE_SIZE), "삼각형");
}

void MainWindow::onSquareClicked() //사각형 그리기
{
  startShape(makePolygon(4, SHAPE_SIZE), "사각형");
}

void MainWindow::onCircleClicked() //원 그리기
{
  startShape(makeCircle(SHAPE_SIZE / 2.0), "원");
}

void MainWindow::onPenChanged() //펜 색상, 굵기 변경
{
  int index = ui->comboColor->currentIndex();
  int width = ui->spinWidth->value();
  qnode->setPen(PEN_COLORS[index][0], PEN_COLORS[index][1], PEN_COLORS[index][2], width);
  setFocus();
}

void MainWindow::onClearClicked() //화면 지우기
{
  qnode->clearScreen();
  setFocus();
}

void MainWindow::closeEvent(QCloseEvent* event) //창 닫을 때 ROS 종료, 스레드 정리
{
  control_timer_->stop();
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
