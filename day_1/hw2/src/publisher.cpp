#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;  // 500ms 같은 시간 표현을 쓰기 위해

class PublisherNode : public rclcpp::Node
{
public:
  // 생성자: 노드 이름 "publisher" 등록 + 퍼블리셔, 타이머 생성
  PublisherNode()
  : Node("publisher"), count_(0)
  {
    // create_publisher<메시지타입>(토픽이름, 큐 크기)
    publisher_ = this->create_publisher<std_msgs::msg::String>("chatter", 10);

    // create_wall_timer(주기, 콜백): 500ms마다 timer_callback() 자동 호출
    timer_ = this->create_wall_timer(500ms, [this]() {timer_callback();});

    RCLCPP_INFO(this->get_logger(), "publisher 노드 시작 (0.5초 주기로 /chatter 발행)");
  }

private:
  // 타이머 콜백: 메시지를 만들어 발행
  void timer_callback()
  {
    auto msg = std_msgs::msg::String();
    msg.data = "Hello ROS2 #" + std::to_string(count_++);

    RCLCPP_INFO(this->get_logger(), "발행: '%s'", msg.data.c_str());
    publisher_->publish(msg);
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  size_t count_;  // 몇 번째 메시지인지
};

// main: 초기화 -> 노드 실행(spin) -> 종료
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PublisherNode>());  // Ctrl+C 전까지 계속 콜백 처리
  rclcpp::shutdown();
  return 0;
}