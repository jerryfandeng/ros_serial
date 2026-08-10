# weather_station

> STM32F407 + AHT20 温湿度数据接入 ROS Noetic 的完整示例：串口采集、正则解析、自定义消息、发布订阅。

## 为什么有这个项目

学 ROS 的时候想到一个问题：机器人是怎么接收传感器数据的？手边正好有
STM32F407 和 AHT20 温湿度模块，于是想验证一条完整链路：

单片机采集数据 → 串口上报 → ROS 节点接收解析 → 自定义消息发布 → 订阅打印。

## 项目功能

- STM32F407 读取 AHT20 温湿度，通过串口按固定格式周期性打印一行 ASCII 日志。
- VMware 把 USB 转 TTL（CH340）直通到 Ubuntu 20.04 虚拟机，串口设备为 `/dev/ttyUSB0`，波特率 115200。
- `weather_publisher`：用 `serial` 库读取串口行数据，用正则提取 `Temperature` / `Humidity`，封装为自定义消息 `WeatherData`，发布到 `/weather_data`。
- `weather_subscriber`：订阅 `/weather_data`，收到数据后用 `ROS_INFO` 打印。
- 格式错误的行会 WARN 丢弃，不影响后续数据。

## 串口日志格式

```text
I/AHT20           [26-08-09 21:24:46] Temperature: 27.2, Humidity: 33.1
```

## 自定义消息

`msg/WeatherData.msg`：

```text
float32 temperature
float32 humidity
```

## 目录结构

```text
.
├── src/
│   └── weather_station/
│       ├── launch/weather.launch
│       ├── msg/WeatherData.msg
│       └── src/weather_publisher.cpp / weather_subscriber.cpp
└── docs/          # 运行截图
```

## 效果截图

| 场景 | 截图文件 |
|------|----------|
| Windows 端 SSCOM 串口日志 | `docs/win_serial.png` |
| Ubuntu 端运行与 `rostopic echo` | `docs/ros_serial.png` |

## 依赖

- Ubuntu 20.04 + ROS Noetic
- `ros-noetic-serial`

```bash
sudo apt update
sudo apt install ros-noetic-serial
```

## 串口权限

永久（需要重新登录生效）：

```bash
sudo usermod -aG dialout $USER
```

当前终端立刻生效用 `newgrp dialout`，否则重新登录后再跑。

临时：

```bash
sudo chmod 666 /dev/ttyUSB0
```

## 编译

如果本目录就是 catkin 工作空间根目录（在 Ubuntu 中挂载为
`/mnt/hgfs/ros_serial`）：

```bash
cd /mnt/hgfs/ros_serial
catkin_make
source devel/setup.bash
```

如果已有 `~/catkin_ws`：

```bash
cp -r /mnt/hgfs/ros_serial/src/weather_station ~/catkin_ws/src/
cd ~/catkin_ws
catkin_make
source devel/setup.bash
```

## 运行

```bash
roslaunch weather_station weather.launch
```

设备是 `/dev/ttyACM0` 时：

```bash
roslaunch weather_station weather.launch port:=/dev/ttyACM0
```

## 验证

另开一个终端：

```bash
source /opt/ros/noetic/setup.bash
source ~/catkin_ws/devel/setup.bash
rostopic echo /weather_data
```

预期输出：

```text
temperature: 27.2
humidity: 33.1
---
```

格式错误的行会打印 WARN 并丢弃，不影响后续数据。
