#include <ros/ros.h>
#include <serial/serial.h>
#include <weather_station/WeatherData.h>

#include <cstdlib>
#include <regex>
#include <string>

namespace {

std::string trim_line_end(const std::string& line) {
  std::string result = line;
  while (!result.empty() &&
         (result.back() == '\r' || result.back() == '\n')) {
    result.pop_back();
  }
  return result;
}

}  // namespace

int main(int argc, char** argv) {
  ros::init(argc, argv, "weather_publisher");
  ros::NodeHandle nh;
  ros::NodeHandle private_nh("~");

  const std::string port = private_nh.param("port", std::string("/dev/ttyUSB0"));
  const int baudrate = private_nh.param("baudrate", 115200);

  ros::Publisher pub =
      nh.advertise<weather_station::WeatherData>("/weather_data", 10);

  serial::Serial serial_port;
  try {
    serial::Timeout timeout = serial::Timeout::simpleTimeout(1000);
    serial_port.setPort(port);
    serial_port.setBaudrate(static_cast<uint32_t>(baudrate));
    serial_port.setTimeout(timeout);
    serial_port.open();
  } catch (const serial::IOException& e) {
    ROS_FATAL("Failed to open serial port %s: %s", port.c_str(), e.what());
    return EXIT_FAILURE;
  } catch (const std::exception& e) {
    ROS_FATAL("Serial setup error on %s: %s", port.c_str(), e.what());
    return EXIT_FAILURE;
  }

  if (!serial_port.isOpen()) {
    ROS_FATAL("Serial port %s is not open", port.c_str());
    return EXIT_FAILURE;
  }
  ROS_INFO("Opened %s at %d baud", port.c_str(), baudrate);

  const std::regex temperature_re(R"(Temperature:\s*([-+]?\d+(?:\.\d+)?))");
  const std::regex humidity_re(R"(Humidity:\s*([-+]?\d+(?:\.\d+)?))");
  std::smatch temperature_match;
  std::smatch humidity_match;

  ros::Rate rate(10.0);
  while (ros::ok() && serial_port.isOpen()) {
    std::string line;
    try {
      line = serial_port.readline(1024, "\n");
    } catch (const serial::IOException& e) {
      ROS_ERROR("Serial read failed: %s", e.what());
      ros::Duration(1.0).sleep();
      continue;
    }

    line = trim_line_end(line);
    if (line.empty()) {
      rate.sleep();
      continue;
    }

    if (!std::regex_search(line, temperature_match, temperature_re) ||
        !std::regex_search(line, humidity_match, humidity_re)) {
      ROS_WARN("Malformed line, discarded: %s", line.c_str());
      rate.sleep();
      continue;
    }

    try {
      weather_station::WeatherData msg;
      msg.temperature = std::stof(temperature_match[1].str());
      msg.humidity = std::stof(humidity_match[1].str());
      pub.publish(msg);
      ROS_INFO("Published temperature=%.1f humidity=%.1f",
               msg.temperature, msg.humidity);
    } catch (const std::exception& e) {
      ROS_WARN("Failed to parse numbers from line: %s (%s)",
               line.c_str(), e.what());
    }

    rate.sleep();
  }

  if (serial_port.isOpen()) {
    serial_port.close();
  }
  return 0;
}
