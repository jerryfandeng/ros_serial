#include <ros/ros.h>
#include <weather_station/WeatherData.h>

void weather_callback(const weather_station::WeatherData::ConstPtr& msg) {
  ROS_INFO("Received temperature=%.1f, humidity=%.1f",
           msg->temperature, msg->humidity);
}

int main(int argc, char** argv) {
  ros::init(argc, argv, "weather_subscriber");
  ros::NodeHandle nh;

  ros::Subscriber sub =
      nh.subscribe("/weather_data", 10, weather_callback);

  ros::spin();
  return 0;
}
