#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "hw3/srv/calc.hpp"  // Calc.srv를 빌드하면 자동 생성되는 헤더

using Calc = hw3::srv::Calc;
using std::placeholders::_1;
using std::placeholders::_2;

class CalcServer : public rclcpp::Node
{
public:
  // 생성자: "/calculator" 서비스를 열고, 요청이 오면 handle_request()가 호출되게 등록
  CalcServer()
  : Node("calc_server")
  {
    service_ = this->create_service<Calc>(
      "calculator", std::bind(&CalcServer::handle_request, this, _1, _2));
    RCLCPP_INFO(this->get_logger(), "calc_server 준비 완료 (/calculator 서비스 대기 중)");
  }

private:
  // 요청 처리 콜백
  //  request  : 클라이언트가 보낸 a, b, op
  //  response : 여기에 result, success, message를 채우면 자동으로 클라이언트에 전송됨
  void handle_request(
    const std::shared_ptr<Calc::Request> request,
    std::shared_ptr<Calc::Response> response)
  {
    const double a = request->a;
    const double b = request->b;
    const std::string & op = request->op;

    response->success = true;

    if (op == "+") {
      response->result = a + b;
    } else if (op == "-") {
      response->result = a - b;
    } else if (op == "x" || op == "*") {
      response->result = a * b;
    } else if (op == "/") {
      if (b == 0.0) {  // 0으로 나누기 예외 처리
        response->success = false;
        response->result = 0.0;
        response->message = "에러: 0으로 나눌 수 없습니다";
      } else {
        response->result = a / b;
      }
    } else {  // 지원하지 않는 연산자 예외 처리
      response->success = false;
      response->result = 0.0;
      response->message = "에러: 지원하지 않는 연산자 '" + op + "' (+, -, x, / 사용)";
    }

    if (response->success) {
      response->message = std::to_string(a) + " " + op + " " + std::to_string(b) +
        " = " + std::to_string(response->result);
    }

    RCLCPP_INFO(
      this->get_logger(), "요청 수신: a=%.2f, op='%s', b=%.2f -> %s",
      a, op.c_str(), b, response->message.c_str());
  }

  rclcpp::Service<Calc>::SharedPtr service_;  // 서비스 서버 핸들
};

// main: 초기화 -> 서버 노드 실행(spin) -> 종료
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CalcServer>());
  rclcpp::shutdown();
  return 0;
}