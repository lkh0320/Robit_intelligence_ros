# Day4_hw2

이규환 2026406017

---

## 1. 개요 (Overview)

`robot_description` 패키지만으로 bringup 패키지를 만들고, CLI 명령으로 로봇의 모든 구동부가 움직이는지 확인하기.

`robot_description` 은 읽기만 하고 수정하지 않았다. 과제 제출을 위해 `robot_description` 의 `.git` 만 제거하여 함께 넣었다.

| 컨트롤러 | 타입 | 관절 |
| --- | --- | --- |
| `joint_state_broadcaster` | joint_state_broadcaster/JointStateBroadcaster | (상태 발행 전용) |
| `camera_tower_controller` | forward_command_controller/ForwardCommandController | ct_joint1_1, ct_joint2_1, ct_joint1_2, ct_joint2_2 |
| `arm_controller` | joint_trajectory_controller/JointTrajectoryController | arm_joint1 ~ arm_joint6 |
| `gripper_controller` | forward_command_controller/ForwardCommandController | gripper_motor_ljoint, gripper_motor_rjoint |
| `flipper_controller` | forward_command_controller/ForwardCommandController | fl_joint, fr_joint, bl_joint, br_joint |
| `diff_drive_controller` | diff_drive_controller/DiffDriveController | left_wheel_joint, right_wheel_joint |

## 2. 체크포인트 (checkpoint)

모든 영상은 `./etc` 폴더 안에 저장되어 있음.

### 2.1 launch 한 번으로 bringup 실행, 모든 컨트롤러 active

<video src="./etc/hw2_Ch1.webm" controls width="600" autoplay loop muted></video>

```bash
ros2 launch eclipse_bringup_lkh bringup.launch.py
ros2 control list_hardware_components
ros2 control list_controllers
```

`eclipse_base`, `eclipse_arm`, `eclipse_arm2`, `eclipse_camera_tower` 4개 하드웨어 컴포넌트가 모두 active 이고, 컨트롤러 6개도 모두 active 로 올라온다. `ros2 control list_hardware_interfaces` 로 확인한 command interface 18개가 전부 claimed 상태다.

### 2.2 카메라 타워 관절 4개에 명령을 보내 움직이기

<video src="./etc/hw2_Ch2.webm" controls width="600" autoplay loop muted></video>

```bash
ros2 topic pub --once /camera_tower_controller/commands \
  std_msgs/msg/Float64MultiArray "{data: [0.5, 0.3, -0.5, -0.3]}"
```

`data` 순서는 YAML 의 `joints` 순서와 같다. rviz 에서 타워 링크 4개가 기울고, `/joint_states` 의 `ct_joint1_1` 0.5, `ct_joint2_1` 0.3, `ct_joint1_2` -0.5, `ct_joint2_2` -0.3 으로 바뀌는 것을 근거로 삼았다.

### 2.3 팔 관절 6개를 컨트롤러 하나로 궤적 명령을 보내 움직이기

<video src="./etc/hw2_Ch3.webm" controls width="600" autoplay loop muted></video>

```bash
ros2 topic pub --once /arm_controller/joint_trajectory \
  trajectory_msgs/msg/JointTrajectory \
  "{joint_names: [arm_joint1, arm_joint2, arm_joint3, arm_joint4, arm_joint5, arm_joint6],
    points: [{positions: [0.3, 0.5, 1.0, 0.2, 0.4, 0.0], time_from_start: {sec: 2}}]}"
```

`arm_joint1` ~ `arm_joint4` 와 `arm_joint6` 은 `eclipse_arm`, `arm_joint5` 는 `eclipse_arm2` 에 나뉘어 있지만 컨트롤러는 인터페이스 이름만 보므로 `arm_controller` 하나로 6관절을 함께 제어할 수 있었다. 2초 동안 보간되며 움직이는 것과 `/arm_controller/controller_state` 의 `reference`·`feedback` 이 6관절 분량으로 나오는 것을 근거로 삼았다.

그리퍼도 같은 방식으로 확인하였다.

```bash
ros2 topic pub --once /gripper_controller/commands \
  std_msgs/msg/Float64MultiArray "{data: [-0.5, 0.5]}"
```

### 2.4 베이스의 바퀴와 플리퍼에 명령을 보내 움직이기

<video src="./etc/hw2_Ch4.webm" controls width="600" autoplay loop muted></video>

```bash
ros2 topic pub --once /diff_drive_controller/cmd_vel \
  geometry_msgs/msg/TwistStamped \
  "{header: {frame_id: base_link}, twist: {linear: {x: 0.3}, angular: {z: 0.0}}}"

ros2 topic pub --once /flipper_controller/commands \
  std_msgs/msg/Float64MultiArray "{data: [0.5, -0.5, -0.5, 0.5]}"
```

바퀴는 velocity 명령이라 `diff_drive_controller` 로 구성하였다. rviz 의 Fixed Frame 을 `odom` 으로 두었으므로 로봇이 실제로 전진하고 제자리 회전하는 것이 보이고, `/diff_drive_controller/odom` 의 `pose.position.x` 가 늘어나는 것을 근거로 삼았다. 플리퍼는 position 명령이라 `/joint_states` 의 `fl_joint` 0.5, `fr_joint` -0.5, `bl_joint` -0.5, `br_joint` 0.5 로 바뀌는 것과 rviz 에서 접히는 모습을 근거로 삼았다.

`/joint_states` 의 일부 값이 `.nan` 으로 나오는 것은 정상이다. `joint_state_broadcaster` 는 모든 관절에 대해 position·velocity·effort 배열을 채우는데, 해당 인터페이스가 `<ros2_control>` 에 선언되지 않은 관절 자리는 NaN 으로 남긴다. 예를 들어 카메라 타워 관절은 effort state 가 없다.

## 3. 간단한 설명

`robot_description` 은 `.git` 을 제외하고는 일절 건드리지 않고, `eclipse_bringup_lkh` 패키지 하나만 만들어 구현하였다. 강의자료의 `controllers.yaml` 과 `bringup.launch.py` 를 바탕으로 컨트롤러를 추가하여 구성하였다.

`diff_drive_controller` 의 `wheel_separation` 과 `wheel_radius` 는 눈대중으로 재지 않고 `eclipse_base.xacro` 에서 직접 읽었다. 바퀴 joint origin 이 `y = ±0.12` 이므로 `wheel_separation` 은 0.24 이고, 바퀴 링크의 cylinder `radius` 가 0.08 이다. Jazzy 에서 `diff_drive_controller` 의 속도 명령은 `geometry_msgs/msg/TwistStamped` 라서 예전 자료처럼 `Twist` 로 보내면 받지 않는다.

`enable_odom_tf` 를 켜 두어 `odom → base_link` TF 가 발행되므로, rviz 의 Fixed Frame 을 `odom` 으로 두어야 로봇이 전진하는 것이 보인다. bringup 의 rviz 설정에 이미 `odom` 으로 넣어 두었다.

`diff_drive_controller` 는 명령이 `cmd_vel_timeout`(기본 0.5초) 동안 오지 않으면 매 제어 주기마다 `Velocity command timed out. Braking.` 경고를 낸다. 100 Hz 로 쌓여 다른 로그를 덮어 버리므로 launch 에서 `ros2_control_node` 에 `--log-level diff_drive_controller:=error` 를 주어 이 컨트롤러의 경고만 내렸다. 정지 동작 자체는 그대로 살아 있다.

과제 명세 외 추가한 부분은 두 가지다. 명세의 CP 는 카메라 타워, 팔, 바퀴와 플리퍼만 요구하지만 그리퍼 모터 2개에도 `gripper_controller` 를 붙여 `<ros2_control>` 에 선언된 command interface 18개를 모두 컨트롤러가 claim 하게 하였다. 또 `joint_state_broadcaster` 가 먼저 active 가 된 뒤에 나머지 컨트롤러를 spawn 하도록 `OnProcessExit` 로 순서를 잡아 spawner 가 동시에 붙어 실패하는 경우를 없앴다.

플리퍼는 좌우 관절 범위가 반대다. `fl_joint`·`br_joint` 는 `[-1.4835, 3.1241]`, `fr_joint`·`bl_joint` 는 `[-3.1241, 1.4835]` 이므로 대칭으로 움직이려면 부호를 반대로 주어야 한다.

실행 영상 `hw2_Ch1.webm` ~ `hw2_Ch4.webm` 도 같은 폴더에 함께 넣어 두었다.
