#!/usr/bin/env bash

echo
echo "=============================================="
echo " 전진 linear.x = 0.3 m/s (3초)"
echo "=============================================="
timeout 3 ros2 topic pub --rate 10 /diff_drive_controller/cmd_vel \
  geometry_msgs/msg/TwistStamped \
  "{header: {frame_id: base_link}, twist: {linear: {x: 0.3}, angular: {z: 0.0}}}"

echo
echo "=============================================="
echo " /diff_drive_controller/odom"
echo "=============================================="
ros2 topic echo /diff_drive_controller/odom --once

echo
echo "=============================================="
echo " 제자리 회전 angular.z = 0.5 rad/s (3초)"
echo "=============================================="
timeout 3 ros2 topic pub --rate 10 /diff_drive_controller/cmd_vel \
  geometry_msgs/msg/TwistStamped \
  "{header: {frame_id: base_link}, twist: {linear: {x: 0.0}, angular: {z: 0.5}}}"

echo
echo "=============================================="
echo " 플리퍼 [0.5, -0.5, -0.5, 0.5]"
echo " (좌우 관절 범위가 반대라서 부호를 반대로 줌)"
echo "=============================================="
ros2 topic pub --once /flipper_controller/commands \
  std_msgs/msg/Float64MultiArray "{data: [0.5, -0.5, -0.5, 0.5]}"
sleep 2

echo
echo "=============================================="
echo " /joint_states"
echo "=============================================="
ros2 topic echo /joint_states --once

echo
echo "odom 의 position.x 가 늘고 플리퍼 position 이 바뀌었으면 CP4 통과"
