#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/char.hpp>
#include <std_srvs/srv/trigger.hpp>

using namespace std::chrono_literals;

class CharPublisher : public rclcpp::Node
{
public:
  CharPublisher() : Node("char_publisher") //퍼블리셔, 타이머, 재시작 서비스 생성
  {
    publisher_ = this->create_publisher<std_msgs::msg::Char>("random_char", 10);
    timer_ = this->create_wall_timer(500ms, std::bind(&CharPublisher::timerCallback, this));
    restart_service_ = this->create_service<std_srvs::srv::Trigger>(
        "char_publisher/restart",
        std::bind(&CharPublisher::restartCallback, this, std::placeholders::_1, std::placeholders::_2));
  }

private:
  void timerCallback() //0.5초마다 랜덤 문자 발행
  {
    std_msgs::msg::Char msg;
    msg.data = 'A' + std::rand() % 26;
    publisher_->publish(msg);
    count_++;
    RCLCPP_INFO(this->get_logger(), "Publish random_char: %c", static_cast<char>(msg.data));
  }

  //재시작 요청 시 타이머 재시작, 발행 횟수 초기화
  void restartCallback(const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
                       std::shared_ptr<std_srvs::srv::Trigger::Response> response)
  {
    (void)request;
    int old_count = count_;
    timer_->cancel();
    count_ = 0;
    timer_->reset();

    response->success = true;
    response->message = "char_publisher 재시작 완료 (재시작 전 발행 " + std::to_string(old_count) + "개)";
    RCLCPP_INFO(this->get_logger(), "Restart requested");
  }

  rclcpp::Publisher<std_msgs::msg::Char>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr restart_service_;
  int count_ = 0;
};

int main(int argc, char** argv) //노드 실행
{
  std::srand(std::time(nullptr) + 3);
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CharPublisher>());
  rclcpp::shutdown();
  return 0;
}
