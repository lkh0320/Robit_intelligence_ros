#!/usr/bin/env bash

echo
echo "=============================================="
echo " [1/3] ros2 lifecycle get /imu_watchdog"
echo "=============================================="
ros2 lifecycle get /imu_watchdog

echo
echo "=============================================="
echo " [2/3] ros2 lifecycle get /fake_imu"
echo "=============================================="
ros2 lifecycle get /fake_imu

echo
echo "=============================================="
echo " [3/3] ros2 topic hz /imu_checked        (5s)"
echo "=============================================="
timeout 5 ros2 topic hz /imu_checked

echo
echo "둘 다 active 이고 50 Hz 가 나오면 2.2 확인 완료"
