#!/usr/bin/env bash

echo
echo "=============================================="
echo " [1/3] ros2 lifecycle get /imu_watchdog  (10s)"
echo "=============================================="
timeout 10 ros2 lifecycle get /imu_watchdog
echo "exit=$?   (124 = 응답 없음 = 데드락)"

echo
echo "=============================================="
echo " [2/3] ros2 service call /fake_imu/get_state"
echo "=============================================="
ros2 service call /fake_imu/get_state lifecycle_msgs/srv/GetState

echo
echo "=============================================="
echo " [3/3] ros2 topic hz /imu_checked        (5s)"
echo "=============================================="
timeout 5 ros2 topic hz /imu_checked

echo
echo "상대 노드는 응답하고 imu_watchdog 만 멈췄으면 2.1 재현 성공"
