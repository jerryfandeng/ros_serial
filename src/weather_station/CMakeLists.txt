cmake_minimum_required(VERSION 3.0.2)
project(weather_station)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  message_generation
  serial
)

add_message_files(
  FILES
  WeatherData.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
  CATKIN_DEPENDS
  roscpp
  std_msgs
  message_runtime
)

include_directories(${catkin_INCLUDE_DIRS})

add_executable(weather_publisher src/weather_publisher.cpp)
add_dependencies(weather_publisher ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(weather_publisher ${catkin_LIBRARIES})

add_executable(weather_subscriber src/weather_subscriber.cpp)
add_dependencies(weather_subscriber ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(weather_subscriber ${catkin_LIBRARIES})
