#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "hw3/srv/calc.hpp"

using namespace std::chrono_literals;
using Calc = hw3::srv::Calc;

// 사용법 안내 출력
void print_usage()
{
  RCLCPP_INFO(rclcpp::get_logger("calc_client"),
    "사용법: ros2 run hw3 calc_client <a> <op> <b>   (op: + - x /)");
}

// main: 인자 확인 -> 요청 만들기 -> 서버 대기 -> 요청 전송 -> 응답 출력
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  // ROS 전용 인자를 뺀 사용자 인자만 추출 (args[0]은 실행 파일 이름)
  std::vector<std::string> args = rclcpp::remove_ros_arguments(argc, argv);
  if (args.size() != 4) {
    print_usage();
    rclcpp::shutdown();
    return 1;
  }

  auto node = rclcpp::Node::make_shared("calc_client");
  auto client = node->create_client<Calc>("calculator");  // 서버와 같은 서비스 이름

  // 요청 메시지 채우기 (숫자가 아니면 안내 후 종료)
  auto request = std::make_shared<Calc::Request>();
  try {
    request->a = std::stod(args[1]);
    request->op = args[2];
    request->b = std::stod(args[3]);
  } catch (const std::exception &) {
    RCLCPP_ERROR(node->get_logger(), "a, b에는 숫자를 입력하세요");
    print_usage();
    rclcpp::shutdown();
    return 1;
  }

  // 서버가 켜질 때까지 1초 간격으로 대기
  while (!client->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(node->get_logger(), "대기 중 종료됨");
      return 1;
    }
    RCLCPP_INFO(node->get_logger(), "calc_server를 기다리는 중...");
  }

  // 비동기로 요청을 보내고, 응답이 올 때까지 대기
  auto future = client->async_send_request(request);
  if (rclcpp::spin_until_future_complete(node, future) ==
    rclcpp::FutureReturnCode::SUCCESS)
  {
    auto response = future.get();
    if (response->success) {
      RCLCPP_INFO(node->get_logger(), "결과: %s", response->message.c_str());
    } else {
      RCLCPP_WARN(node->get_logger(), "계산 실패: %s", response->message.c_str());
    }
  } else {
    RCLCPP_ERROR(node->get_logger(), "서비스 호출 실패");
  }

  rclcpp::shutdown();
  return 0;
}