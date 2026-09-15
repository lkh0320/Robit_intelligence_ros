#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "hw3/msg/my_msg.hpp"

class MyMsgSubscriber : public rclcpp::Node
{
public:
  // 생성자: /my_msg 구독 등록 (콜백은 람다 함수로 작성)
  MyMsgSubscriber()
  : Node("mymsg_subscriber")
  {
    subscription_ = this->create_subscription<hw3::msg::MyMsg>(
      "my_msg", 10,
      // 수신 콜백: 메시지가 올 때마다 두 필드를 출력
      [this](const hw3::msg::MyMsg & msg) {
        RCLCPP_INFO(this->get_logger(), "수신: id=%d, label='%s'", msg.id, msg.label.c_str());
      });
    RCLCPP_INFO(this->get_logger(), "mymsg_subscriber 노드 시작 (/my_msg 구독 중)");
  }

private:
  rclcpp::Subscription<hw3::msg::MyMsg>::SharedPtr subscription_;
};

// main: 초기화 -> 노드 실행(spin) -> 종료
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MyMsgSubscriber>());
  rclcpp::shutdown();
  return 0;
}