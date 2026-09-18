cmake_minimum_required(VERSION 3.8)
project(day4_hw1)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(lifecycle_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(fake_imu src/fake_imu.cpp)
target_compile_features(fake_imu PUBLIC cxx_std_17)
target_include_directories(fake_imu PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(fake_imu PUBLIC
  rclcpp::rclcpp
  rclcpp_lifecycle::rclcpp_lifecycle
  ${lifecycle_msgs_TARGETS}
  ${sensor_msgs_TARGETS}
)

add_executable(imu_watchdog src/imu_watchdog.cpp)
target_compile_features(imu_watchdog PUBLIC cxx_std_17)
target_include_directories(imu_watchdog PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(imu_watchdog PUBLIC
  rclcpp::rclcpp
  rclcpp_lifecycle::rclcpp_lifecycle
  ${lifecycle_msgs_TARGETS}
  ${sensor_msgs_TARGETS}
)

add_executable(imu_watchdog_sync src/imu_watchdog_sync.cpp)
target_compile_features(imu_watchdog_sync PUBLIC cxx_std_17)
target_include_directories(imu_watchdog_sync PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(imu_watchdog_sync PUBLIC
  rclcpp::rclcpp
  rclcpp_lifecycle::rclcpp_lifecycle
  ${lifecycle_msgs_TARGETS}
  ${sensor_msgs_TARGETS}
)

add_executable(imu_watchdog_mt src/imu_watchdog_mt.cpp)
target_compile_features(imu_watchdog_mt PUBLIC cxx_std_17)
target_include_directories(imu_watchdog_mt PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(imu_watchdog_mt PUBLIC
  rclcpp::rclcpp
  rclcpp_lifecycle::rclcpp_lifecycle
  ${lifecycle_msgs_TARGETS}
  ${sensor_msgs_TARGETS}
)

install(TARGETS fake_imu imu_watchdog imu_watchdog_sync imu_watchdog_mt
  DESTINATION lib/${PROJECT_NAME})
install(PROGRAMS
  scripts/ch1_deadlock.sh
  scripts/ch2_check.sh
  scripts/ch3_states.sh
  DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

ament_package()
