#include "day4_hw1/fake_imu.hpp"

FakeImu::FakeImu(const rclcpp::NodeOptions & options)
: rclcpp_lifecycle::LifecycleNode("fake_imu", options)
{
  declare_parameter("rate_hz", 50.0); //발행 주기
  declare_parameter("stamp_offset_sec", 0.0); //스탬프를 과거로 미는 양
  declare_parameter("frame_id", "imu_link"); //메시지 frame_id
}

CallbackReturn FakeImu::on_configure(const rclcpp_lifecycle::State &)
{
  rate_hz_ = get_parameter("rate_hz").as_double(); //파라미터 읽기
  frame_id_ = get_parameter("frame_id").as_string();
  const double offset = get_parameter("stamp_offset_sec").as_double();

  if (rate_hz_ <= 0.0) {
    RCLCPP_ERROR(get_logger(), "rate_hz must be > 0 (got %.2f)", rate_hz_);
    return CallbackReturn::FAILURE; //Unconfigured로 복귀
  }
  if (offset < 0.0) {
    RCLCPP_ERROR(get_logger(), "stamp_offset_sec must be >= 0 (got %.2f)", offset);
    return CallbackReturn::FAILURE;
  }

  pub_ = create_publisher<sensor_msgs::msg::Imu>("imu", rclcpp::SensorDataQoS()); //LifecyclePublisher 생성
  count_ = 0;
  RCLCPP_INFO(get_logger(), "on_configure: rate = %.1f Hz, offset = %.3f s", rate_hz_, offset);
  return CallbackReturn::SUCCESS;
}

CallbackReturn FakeImu::on_activate(const rclcpp_lifecycle::State & state)
{
  LifecycleNode::on_activate(state); //publisher 활성화
  timer_ = create_timer(
    std::chrono::duration<double>(1.0 / rate_hz_),
    [this]() {publish_imu();}); //노드 clock 기준 타이머
  RCLCPP_INFO(get_logger(), "on_activate: publishing /imu");
  return CallbackReturn::SUCCESS;
}

CallbackReturn FakeImu::on_deactivate(const rclcpp_lifecycle::State & state)
{
  LifecycleNode::on_deactivate(state); //publisher 비활성화
  timer_.reset(); //타이머 정지
  RCLCPP_INFO(get_logger(), "on_deactivate: /imu stopped");
  return CallbackReturn::SUCCESS;
}

CallbackReturn FakeImu::on_cleanup(const rclcpp_lifecycle::State &)
{
  timer_.reset();
  pub_.reset(); //configure에서 만든 것 해제
  RCLCPP_INFO(get_logger(), "on_cleanup");
  return CallbackReturn::SUCCESS;
}

CallbackReturn FakeImu::on_shutdown(const rclcpp_lifecycle::State & previous_state)
{
  timer_.reset();
  pub_.reset();
  RCLCPP_INFO(get_logger(), "on_shutdown from [%s]", previous_state.label().c_str());
  return CallbackReturn::SUCCESS;
}

CallbackReturn FakeImu::on_error(const rclcpp_lifecycle::State & previous_state)
{
  RCLCPP_ERROR(get_logger(), "on_error from [%s]", previous_state.label().c_str());
  timer_.reset();
  pub_.reset();
  return CallbackReturn::SUCCESS; //Unconfigured로 복구
}

void FakeImu::publish_imu()
{
  const double offset = get_parameter("stamp_offset_sec").as_double(); //실행 중 param set 반영
  const rclcpp::Time t = now();
  if (t.seconds() < offset) {
    return; //음수 시간 방지
  }

  sensor_msgs::msg::Imu msg;
  msg.header.stamp = t - rclcpp::Duration::from_seconds(offset); //stamp = now() - offset
  msg.header.frame_id = frame_id_;

  const double phase = 0.1 * static_cast<double>(count_);
  msg.orientation.w = 1.0; //회전 없음
  msg.angular_velocity.z = 0.5 * std::sin(phase); //가짜 각속도
  msg.linear_acceleration.x = 0.2 * std::cos(phase); //가짜 가속도
  msg.linear_acceleration.z = 9.81; //중력

  pub_->publish(msg);
  count_++;
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<FakeImu>();
  rclcpp::spin(node->get_node_base_interface()); //LifecycleNode는 base interface로 spin
  rclcpp::shutdown();
  return 0;
}
