#include "../include/day2_hw1/qnode.hpp"

QNode::QNode() //ROS 초기화, 퍼블리셔·서브스크라이버·서비스 클라이언트 생성 후 스레드 시작
{
  int argc = 0;
  char** argv = NULL;
  rclcpp::init(argc, argv);
  node = rclcpp::Node::make_shared("day2_hw1");

  cmd_vel_pub_ = node->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

  cmd_vel_sub_ = node->create_subscription<geometry_msgs::msg::Twist>(
      "/turtle1/cmd_vel", 10,
      [this](const geometry_msgs::msg::Twist& msg) { Q_EMIT cmdVelReceived(msg.linear.x, msg.angular.z); });

  set_pen_client_ = node->create_client<turtlesim::srv::SetPen>("/turtle1/set_pen");
  clear_client_ = node->create_client<std_srvs::srv::Empty>("/clear");

  this->start();
}

QNode::~QNode() //ROS 종료 후 스레드가 끝날 때까지 대기
{
  if (rclcpp::ok())
  {
    rclcpp::shutdown();
  }
  wait();
}

void QNode::run() //ROS 스레드에서 20Hz로 콜백 처리
{
  rclcpp::WallRate loop_rate(20);
  while (rclcpp::ok())
  {
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }
  rclcpp::shutdown();
  Q_EMIT rosShutDown();
}

void QNode::publishCmdVel(double linear, double angular) //cmd_vel 발행
{
  geometry_msgs::msg::Twist msg;
  msg.linear.x = linear;
  msg.angular.z = angular;
  cmd_vel_pub_->publish(msg);
}

void QNode::setPen(int r, int g, int b, int width) //set_pen 서비스로 펜 색상, 굵기 변경
{
  if (!set_pen_client_->service_is_ready())
  {
    return;
  }
  auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
  request->r = r;
  request->g = g;
  request->b = b;
  request->width = width;
  request->off = 0;
  set_pen_client_->async_send_request(request);
}

void QNode::clearScreen() //clear 서비스로 화면 지우기
{
  if (!clear_client_->service_is_ready())
  {
    return;
  }
  auto request = std::make_shared<std_srvs::srv::Empty::Request>();
  clear_client_->async_send_request(request);
}
