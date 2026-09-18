#include "day4_hw1/imu_watchdog_sync.hpp"

ImuWatchdogSync::ImuWatchdogSync(const rclcpp::NodeOptions & options)
: rclcpp_lifecycle::LifecycleNode("imu_watchdog", options)
{
  declare_parameter("timeout_sec", 0.5); //이보다 오래되면 STALE
  declare_parameter("check_rate_hz", 5.0); //검사 주기
  declare_parameter("use_wall_timer", false); //true면 create_wall_timer 사용
  declare_parameter("state_period_sec", 1.0); //fake_imu 상태 조회 주기
  declare_parameter("log_age", true); //정상일 때도 매 주기 age 출력
}

CallbackReturn ImuWatchdogSync::on_configure(const rclcpp_lifecycle::State &)
{
  timeout_sec_ = get_parameter("timeout_sec").as_double(); //파라미터 읽기
  check_rate_hz_ = get_parameter("check_rate_hz").as_double();
  use_wall_timer_ = get_parameter("use_wall_timer").as_bool();
  state_period_sec_ = get_parameter("state_period_sec").as_double();
  log_age_ = get_parameter("log_age").as_bool();

  if (timeout_sec_ <= 0.0) {
    RCLCPP_ERROR(get_logger(), "timeout_sec must be > 0 (got %.3f)", timeout_sec_);
    return CallbackReturn::FAILURE; //Unconfigured로 복귀
  }
  if (check_rate_hz_ <= 0.0) {
    RCLCPP_ERROR(get_logger(), "check_rate_hz must be > 0 (got %.3f)", check_rate_hz_);
    return CallbackReturn::FAILURE;
  }
  if (state_period_sec_ <= 0.0) {
    RCLCPP_ERROR(get_logger(), "state_period_sec must be > 0 (got %.3f)", state_period_sec_);
    return CallbackReturn::FAILURE;
  }
  timeout_ = rclcpp::Duration::from_seconds(timeout_sec_); //검증 후 Duration으로 저장

  checked_pub_ = create_publisher<sensor_msgs::msg::Imu>(
    "imu_checked", rclcpp::SensorDataQoS()); //신선한 데이터 재발행용

  state_client_ = create_client<GetState>("/fake_imu/get_state"); //센서 노드 상태 조회용

  RCLCPP_INFO(
    get_logger(), "on_configure: timeout = %.3f s, check = %.1f Hz, timer = %s, state = %.1f s",
    timeout_sec_, check_rate_hz_, use_wall_timer_ ? "wall" : "ros", state_period_sec_);
  return CallbackReturn::SUCCESS;
}

CallbackReturn ImuWatchdogSync::on_activate(const rclcpp_lifecycle::State & state)
{
  LifecycleNode::on_activate(state); //imu_checked publisher 활성화

  received_ = false; //감시 상태 초기화
  stale_ = false;
  no_data_logged_ = false;

  imu_sub_ = create_subscription<sensor_msgs::msg::Imu>(
    "imu", rclcpp::SensorDataQoS(),
    [this](const sensor_msgs::msg::Imu::ConstSharedPtr & msg) {on_imu(msg);}); //활성화 때 구독 생성

  const auto period = std::chrono::duration<double>(1.0 / check_rate_hz_);
  if (use_wall_timer_) {
    check_timer_ = create_wall_timer(period, [this]() {check_imu();}); //steady clock 기준
  } else {
    check_timer_ = create_timer(period, [this]() {check_imu();}); //노드 clock 기준
  }

  state_timer_ = create_timer(
    std::chrono::duration<double>(state_period_sec_),
    [this]() {on_state_timer();}); //상태 조회 타이머 시작

  RCLCPP_INFO(get_logger(), "on_activate: watching /imu");
  return CallbackReturn::SUCCESS;
}

CallbackReturn ImuWatchdogSync::on_deactivate(const rclcpp_lifecycle::State & state)
{
  LifecycleNode::on_deactivate(state); //publisher 비활성화
  check_timer_.reset(); //타이머 정지
  state_timer_.reset(); //상태 조회 타이머 정지
  imu_sub_.reset(); //구독 해제
  RCLCPP_INFO(get_logger(), "on_deactivate");
  return CallbackReturn::SUCCESS;
}

CallbackReturn ImuWatchdogSync::on_cleanup(const rclcpp_lifecycle::State &)
{
  check_timer_.reset();
  state_timer_.reset();
  imu_sub_.reset();
  checked_pub_.reset(); //configure에서 만든 것 해제
  state_client_.reset(); //configure에서 만든 클라이언트 해제
  RCLCPP_INFO(get_logger(), "on_cleanup");
  return CallbackReturn::SUCCESS;
}

CallbackReturn ImuWatchdogSync::on_shutdown(const rclcpp_lifecycle::State & previous_state)
{
  check_timer_.reset();
  state_timer_.reset();
  imu_sub_.reset();
  checked_pub_.reset();
  state_client_.reset();
  RCLCPP_INFO(get_logger(), "on_shutdown from [%s]", previous_state.label().c_str());
  return CallbackReturn::SUCCESS;
}

CallbackReturn ImuWatchdogSync::on_error(const rclcpp_lifecycle::State & previous_state)
{
  RCLCPP_ERROR(get_logger(), "on_error from [%s]", previous_state.label().c_str());
  check_timer_.reset();
  state_timer_.reset();
  imu_sub_.reset();
  checked_pub_.reset();
  state_client_.reset();
  return CallbackReturn::SUCCESS; //Unconfigured로 복구
}

void ImuWatchdogSync::on_imu(const sensor_msgs::msg::Imu::ConstSharedPtr & msg)
{
  last_stamp_ = rclcpp::Time(msg->header.stamp, get_clock()->get_clock_type()); //노드 clock 타입으로 변환
  received_ = true;

  const rclcpp::Duration age = now() - last_stamp_; //스탬프 기준 데이터 나이
  if (age <= timeout_) {
    checked_pub_->publish(*msg); //신선한 데이터만 재발행
  }
}

void ImuWatchdogSync::check_imu()
{
  if (now().nanoseconds() == 0) {
    return; //sim time에서 /clock 받기 전
  }

  if (!received_) {
    if (!no_data_logged_) {
      RCLCPP_WARN(get_logger(), "[NO DATA] /imu not received yet"); //한 번만 출력
      no_data_logged_ = true;
    }
    return;
  }

  const rclcpp::Duration age = now() - last_stamp_; //마지막 스탬프 후 지난 시간
  if (age > timeout_) {
    RCLCPP_WARN(get_logger(), "[STALE] age = %.2f s", age.seconds()); //출력할 때만 초로 변환
    stale_ = true;
  } else if (stale_) {
    RCLCPP_INFO(get_logger(), "[RECOVERED] age = %.2f s", age.seconds()); //STALE에서 정상 복귀
    stale_ = false;
  } else if (log_age_) {
    RCLCPP_INFO(get_logger(), "[OK] age = %.2f s", age.seconds()); //로그가 멈추는 것을 눈으로 보려고 매 주기 출력
  }
}

void ImuWatchdogSync::on_state_timer()
{
  if (!state_client_->service_is_ready()) {
    RCLCPP_WARN(get_logger(), "fake_imu get_state not available"); //서비스가 없으면 호출하지 않음
    return;
  }

  auto request = std::make_shared<GetState::Request>();
  auto future = state_client_->async_send_request(request); //요청 보내기
  RCLCPP_INFO(get_logger(), "waiting for fake_imu get_state response"); //여기까지는 출력됨
  future.wait(); //콜백 안에서 응답 완료를 기다림, 응답 처리도 이 executor의 일이라 서로 기다리며 멈춤
  RCLCPP_INFO(
    get_logger(), "fake_imu state: %s",
    future.get()->current_state.label.c_str()); //이 줄은 출력되지 않음
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ImuWatchdogSync>();
  rclcpp::spin(node->get_node_base_interface()); //LifecycleNode는 base interface로 spin
  rclcpp::shutdown();
  return 0;
}
