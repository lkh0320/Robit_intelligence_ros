#!/usr/bin/env bash

echo
echo "=============================================="
echo " 팔 6관절 궤적 [0.3, 0.5, 1.0, 0.2, 0.4, 0.0] / 2초"
echo "=============================================="
ros2 topic pub --once /arm_controller/joint_trajectory \
  trajectory_msgs/msg/JointTrajectory \
  "{joint_names: [arm_joint1, arm_joint2, arm_joint3, arm_joint4, arm_joint5, arm_joint6],
    points: [{positions: [0.3, 0.5, 1.0, 0.2, 0.4, 0.0], time_from_start: {sec: 2}}]}"
sleep 3

echo
echo "=============================================="
echo " /arm_controller/controller_state"
echo "=============================================="
ros2 topic echo /arm_controller/controller_state --once

echo
echo "=============================================="
echo " 그리퍼 [-0.5, 0.5]"
echo "=============================================="
ros2 topic pub --once /gripper_controller/commands \
  std_msgs/msg/Float64MultiArray "{data: [-0.5, 0.5]}"
sleep 2

echo
echo "=============================================="
echo " /joint_states"
echo "=============================================="
ros2 topic echo /joint_states --once

echo
echo "컨트롤러 하나가 arm_joint1~6 을 모두 들고 움직였으면 CP3 통과"
