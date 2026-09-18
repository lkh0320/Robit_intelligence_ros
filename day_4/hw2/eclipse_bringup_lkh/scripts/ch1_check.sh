#!/usr/bin/env bash

echo
echo "=============================================="
echo " [1/3] ros2 control list_hardware_components"
echo "=============================================="
ros2 control list_hardware_components

echo
echo "=============================================="
echo " [2/3] ros2 control list_controllers"
echo "=============================================="
ros2 control list_controllers

echo
echo "=============================================="
echo " [3/3] ros2 control list_hardware_interfaces"
echo "=============================================="
ros2 control list_hardware_interfaces

echo
echo "컴포넌트 4개와 컨트롤러 6개가 모두 active 이면 CP1 통과"
