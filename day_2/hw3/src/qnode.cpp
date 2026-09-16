#include "../include/day2_hw3/qnode.hpp"

QNode::QNode() //ROS 초기화, 토픽 3개 구독, 재시작 서비스 클라이언트 생성 후 스레드 시작
{
  int argc = 0;
  char** argv = NULL;
  rclcpp::init(argc, argv);
  node = rclcpp::Node::make_shared("day2_hw3");

  int_sub_ = node->create_subscription<std_msgs::msg::Int32>(
      "random_int", 10, [this](const std_msgs::msg::Int32& msg) { Q_EMIT intReceived(msg.data); });

  bool_sub_ = node->create_subscription<std_msgs::msg::Bool>(
      "random_bool", 10, [this](const std_msgs::msg::Bool& msg) { Q_EMIT boolReceived(msg.data); });

  char_sub_ = node->create_subscription<std_msgs::msg::Char>(
      "random_char", 10, [this](const std_msgs::msg::Char& msg) { Q_EMIT charReceived(QString(QChar(msg.data))); });

  restart_clients_[0] = node->create_client<std_srvs::srv::Trigger>("int_publisher/restart");
  restart_clients_[1] = node->create_client<std_srvs::srv::Trigger>("bool_publisher/restart");
  restart_clients_[2] = node->create_client<std_srvs::srv::Trigger>("char_publisher/restart");

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

void QNode::requestRestart(int index) //선택한 퍼블리셔 노드에 재시작 서비스 요청
{
  if (index < 0 || index > 2)
  {
    return;
  }

  auto client = restart_clients_[index];
  if (!client->service_is_ready())
  {
    Q_EMIT serviceResponded(QString(client->get_service_name()) + " 서비스 서버가 없음 (노드가 꺼져 있음)");
    return;
  }

  auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
  client->async_send_request(request, [this](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future) {
    auto response = future.get();
    Q_EMIT serviceResponded(QString::fromStdString(response->message));
  });
}
