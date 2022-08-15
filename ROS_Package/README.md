Snapbot ROS Package

"src" folder contains two ROS packages used for Snapbop.

1. Dynamixel SDK 
 Snapbot has Dynamixel XM430-W210-T servo motors as its actuators. To run the servo motors, "Snapbot" package uses Dynamixel SDK. 
 
2. Snapbot ROS package
 This package is to move Snapbot with basic locomotion capability in various configurations. Currently, the Camera function is under development and disabled for the configuration recognition. Please connect the six legs to increase the motor ID counterclockwise when starting the robot. It will follow the pre-programmed configuration.
 
 List of command
 
 w: Forward \\
 s: Backword \\
 a: Steer left
 d: Steer right
 q: Diagonal left front
 e: Diagonal right front
 z: Diagonal left back
 c: Diagonal right back
 t: Rotate counterclockwise
 r: Rotate clockwise
 v: Faster
 b: Slower
 1 & 2: change leg angles 
 3 & 4: change leg distance from the body center
 g: reset configuration
 
