cmake_minimum_required(VERSION 3.8)
project(eclipse_bringup_lkh)

find_package(ament_cmake REQUIRED)

install(
  DIRECTORY config launch
  DESTINATION share/${PROJECT_NAME}
)

install(PROGRAMS
  scripts/ch1_check.sh
  scripts/ch2_camera_tower.sh
  scripts/ch3_arm.sh
  scripts/ch4_base.sh
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
