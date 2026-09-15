cmake_minimum_required(VERSION 3.8)
project(hw3)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---- 사용할 패키지 찾기 ----
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_default_generators REQUIRED)   # msg/srv -> C++ 코드 자동 생성기

# ---- 커스텀 인터페이스 등록 ----
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/MyMsg.msg"
  "srv/Calc.srv"
)
ament_export_dependencies(rosidl_default_runtime)

# 같은 패키지 안의 노드가 방금 만든 msg/srv를 쓸 수 있게 연결 대상 가져오기
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} rosidl_typesupport_cpp)

# ---- [실습과제 3] MyMsg 발행/구독 ----
add_executable(mymsg_publisher src/mymsg_publisher.cpp)
ament_target_dependencies(mymsg_publisher rclcpp)
target_link_libraries(mymsg_publisher "${cpp_typesupport_target}")

add_executable(mymsg_subscriber src/mymsg_subscriber.cpp)
ament_target_dependencies(mymsg_subscriber rclcpp)
target_link_libraries(mymsg_subscriber "${cpp_typesupport_target}")

# ---- [2p-1] 계산기 서비스 ----
add_executable(calc_server src/calc_server.cpp)
ament_target_dependencies(calc_server rclcpp)
target_link_libraries(calc_server "${cpp_typesupport_target}")

add_executable(calc_client src/calc_client.cpp)
ament_target_dependencies(calc_client rclcpp)
target_link_libraries(calc_client "${cpp_typesupport_target}")

# ---- 설치 ----
install(TARGETS
  mymsg_publisher
  mymsg_subscriber
  calc_server
  calc_client
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()