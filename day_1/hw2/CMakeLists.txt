cmake_minimum_required(VERSION 3.8)
project(hw2)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---- 사용할 패키지 찾기 ----
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# ---- 실행 파일 등록 (hw1과 같은 노드를 hw2 안에서도 빌드) ----
add_executable(publisher src/publisher.cpp)
ament_target_dependencies(publisher rclcpp std_msgs)

add_executable(subscriber src/subscriber.cpp)
ament_target_dependencies(subscriber rclcpp std_msgs)

# ---- 실행 파일 설치 ----
install(TARGETS
  publisher
  subscriber
  DESTINATION lib/${PROJECT_NAME}
)

# ---- launch 폴더 설치 -> ros2 launch hw2 pubsub_launch.py ----
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()