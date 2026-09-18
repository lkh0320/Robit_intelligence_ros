#ifndef DAY4_HW1__FAKE_IMU_HPP_
#define DAY4_HW1__FAKE_IMU_HPP_

#include <chrono>
#include <cmath>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp_lifecycle/lifecycle_publisher.hpp"
#include "sensor_msgs/msg/imu.hpp"

using CallbackReturn =
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class FakeImu : public rclcpp_lifecycle::LifecycleNode
{
public:
  explicit FakeImu(const rclcpp::NodeOptions & options = rclcpp::NodeOptions()); //파라미터 선언만

  CallbackReturn on_configure(const rclcpp_lifecycle::State &) override; //파라미터 검증, publisher 생성
  CallbackReturn on_activate(const rclcpp_lifecycle::State & state) override; //발행 타이머 시작
  CallbackReturn on_deactivate(const rclcpp_lifecycle::State & state) override; //발행 타이머 정지
  CallbackReturn on_cleanup(const rclcpp_lifecycle::State &) override; //자원 해제
  CallbackReturn on_shutdown(const rclcpp_lifecycle::State & previous_state) override; //종료 시 자원 해제
  CallbackReturn on_error(const rclcpp_lifecycle::State & previous_state) override; //에러 시 정리

private:
  void publish_imu(); //가짜 IMU 한 번 발행

  double rate_hz_{0.0}; //발행 주기
  std::string frame_id_; //메시지 frame_id
  std::size_t count_{0}; //발행 횟수
  rclcpp_lifecycle::LifecyclePublisher<sensor_msgs::msg::Imu>::SharedPtr pub_; //imu 토픽 발행자
  rclcpp::TimerBase::SharedPtr timer_; //발행 타이머
};

#endif
