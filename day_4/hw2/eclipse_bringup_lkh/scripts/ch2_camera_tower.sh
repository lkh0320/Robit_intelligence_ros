#!/usr/bin/env bash

echo
echo "=============================================="
echo " 카메라 타워 4관절 [0.5, 0.3, -0.5, -0.3]"
echo " (순서 = ct_joint1_1, ct_joint2_1, ct_joint1_2, ct_joint2_2)"
echo "=============================================="
ros2 topic pub --once /camera_tower_controller/commands \
  std_msgs/msg/Float64MultiArray "{data: [0.5, 0.3, -0.5, -0.3]}"
sleep 2

echo
echo "=============================================="
echo " /joint_states"
echo "=============================================="
ros2 topic echo /joint_states --once

echo
echo "rviz 의 타워가 기울고 위 position 이 명령값과 같으면 CP2 통과"
