cmake_minimum_required(VERSION 3.5)
project(day2_hw3)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Boost 1.71.0 REQUIRED COMPONENTS thread)
find_package(Qt5 COMPONENTS Widgets Core Gui Network REQUIRED)
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(
  rclcpp
  REQUIRED
)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)

file(GLOB PRO_SOURCE_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  src/*.cpp)
file(GLOB PRO_UI_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  ui/*.ui)
file(GLOB PRO_INCLUDE_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  include/${PROJECT_NAME}/*.h include/${PROJECT_NAME}/*.hpp)
file(GLOB PRO_RESOURCE_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  resource/*.qrc)
qt5_wrap_ui(QT_UI_HPP ${PRO_UI_DIR})
qt5_wrap_cpp(QT_MOC_HPP ${PRO_INCLUDE_DIR})
qt5_add_resources(QT_RESOUCE_HPP ${PRO_RESOURCE_DIR})
include_directories(
	include/
	${CMAKE_CURRENT_BINARY_DIR}
)
add_executable(${PROJECT_NAME}
    ${PRO_SOURCE_DIR}
    ${PRO_INCLUDE_DIR}
    ${PRO_UI_DIR}
    ${QT_MOC_HPP}
    ${QT_RESOUCE_HPP}
    ${QT_UI_HPP} 
  )
ament_target_dependencies(${PROJECT_NAME} rclcpp std_msgs std_srvs Boost)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network ${OpenCV_LIBRARIES})
install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})
add_executable(int_publisher nodes/int_publisher.cpp)
ament_target_dependencies(int_publisher rclcpp std_msgs std_srvs)
add_executable(bool_publisher nodes/bool_publisher.cpp)
ament_target_dependencies(bool_publisher rclcpp std_msgs std_srvs)
add_executable(char_publisher nodes/char_publisher.cpp)
ament_target_dependencies(char_publisher rclcpp std_msgs std_srvs)
install(TARGETS int_publisher bool_publisher char_publisher DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()