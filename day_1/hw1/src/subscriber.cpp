#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;

class SubscriberNode : public rclcpp::Node
{
public:
  // 생성자: 노드 이름 "subscriber" 등록 + /chatter 구독 시작
  SubscriberNode()
  : Node("subscriber")
  {
    // create_subscription<메시지타입>(토픽이름, 큐 크기, 콜백)
    // 메시지가 도착할 때마다 topic_callback()이 호출됨
    subscription_ = this->create_subscription<std_msgs::msg::String>(
      "chatter", 10, std::bind(&SubscriberNode::topic_callback, this, _1));

    RCLCPP_INFO(this->get_logger(), "subscriber 노드 시작 (/chatter 구독 중)");
  }

private:
  // 구독 콜백: 받은 문자열 출력
  void topic_callback(const std_msgs::msg::String & msg) const
  {
    RCLCPP_INFO(this->get_logger(), "수신: '%s'", msg.data.c_str());
  }

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

// main: 초기화 -> 노드 실행(spin) -> 종료
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SubscriberNode>());
  rclcpp::shutdown();
  return 0;
}