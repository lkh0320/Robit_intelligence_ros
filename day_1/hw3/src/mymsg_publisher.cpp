#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "hw3/msg/my_msg.hpp"  // MyMsg.msg를 빌드하면 자동 생성되는 헤더 (이름은 snake_case)

using namespace std::chrono_literals;

class MyMsgPublisher : public rclcpp::Node
{
public:
  // 생성자: MyMsg 타입 퍼블리셔와 0.5초 타이머 생성
  MyMsgPublisher()
  : Node("mymsg_publisher"), id_(0)
  {
    publisher_ = this->create_publisher<hw3::msg::MyMsg>("my_msg", 10);
    timer_ = this->create_wall_timer(500ms, [this]() {timer_callback();});
    RCLCPP_INFO(this->get_logger(), "mymsg_publisher 노드 시작 (/my_msg 발행)");
  }

private:
  // 타이머 콜백: MyMsg의 두 필드(id, label)를 채워서 발행
  void timer_callback()
  {
    hw3::msg::MyMsg msg;
    msg.id = id_;
    msg.label = "robot_status_" + std::to_string(id_);
    id_++;

    RCLCPP_INFO(this->get_logger(), "발행: id=%d, label='%s'", msg.id, msg.label.c_str());
    publisher_->publish(msg);
  }

  rclcpp::Publisher<hw3::msg::MyMsg>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int32_t id_;  // 발행할 때마다 1씩 증가
};

// main: 초기화 -> 노드 실행(spin) -> 종료
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MyMsgPublisher>());
  rclcpp::shutdown();
  return 0;
}