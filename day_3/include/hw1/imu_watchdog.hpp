#ifndef HW1__IMU_WATCHDOG_HPP_
#define HW1__IMU_WATCHDOG_HPP_

#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp_lifecycle/lifecycle_publisher.hpp"
#include "sensor_msgs/msg/imu.hpp"

using CallbackReturn =
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class ImuWatchdog : public rclcpp_lifecycle::LifecycleNode
{
public:
  explicit ImuWatchdog(const rclcpp::NodeOptions & options = rclcpp::NodeOptions()); //파라미터 선언만

  CallbackReturn on_configure(const rclcpp_lifecycle::State &) override; //파라미터 검증, publisher 생성
  CallbackReturn on_activate(const rclcpp_lifecycle::State & state) override; //구독, 검사 타이머 시작
  CallbackReturn on_deactivate(const rclcpp_lifecycle::State & state) override; //구독, 검사 타이머 정지
  CallbackReturn on_cleanup(const rclcpp_lifecycle::State &) override; //자원 해제
  CallbackReturn on_shutdown(const rclcpp_lifecycle::State & previous_state) override; //종료 시 자원 해제
  CallbackReturn on_error(const rclcpp_lifecycle::State & previous_state) override; //에러 시 정리

private:
  void on_imu(const sensor_msgs::msg::Imu::ConstSharedPtr & msg); //imu 토픽 수신 콜백
  void check_imu(); //주기적으로 age 검사

  double timeout_sec_{0.5}; //이보다 오래되면 STALE
  rclcpp::Duration timeout_{0, 0}; //비교용 Duration
  double check_rate_hz_{5.0}; //검사 주기
  bool use_wall_timer_{false}; //wall timer 사용 여부

  bool received_{false}; //한 번이라도 받았는지
  bool stale_{false}; //현재 STALE 상태인지
  bool no_data_logged_{false}; //NO DATA를 이미 출력했는지
  rclcpp::Time last_stamp_{0, 0, RCL_ROS_TIME}; //마지막 메시지 스탬프

  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_; //imu 토픽 구독자
  rclcpp_lifecycle::LifecyclePublisher<sensor_msgs::msg::Imu>::SharedPtr checked_pub_; //imu_checked 토픽 발행자
  rclcpp::TimerBase::SharedPtr check_timer_; //검사 타이머
};

#endif
