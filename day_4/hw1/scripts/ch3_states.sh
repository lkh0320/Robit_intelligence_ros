#!/usr/bin/env bash

GAP="${1:-3}"

for t in deactivate cleanup configure activate shutdown; do
  echo
  echo "=============================================="
  echo " ros2 lifecycle set /fake_imu $t"
  echo "=============================================="
  ros2 lifecycle set /fake_imu "$t"
  sleep "$GAP"
done

echo
echo "=============================================="
echo " pkill -f day4_hw1/fake_imu"
echo "=============================================="
pkill -f day4_hw1/fake_imu
sleep "$GAP"

echo
echo "imu_watchdog 로그가 inactive -> unconfigured -> inactive -> active -> finalized"
echo "를 따라가고 마지막에 'get_state not available' 로 바뀌면 2.3 확인 완료"
