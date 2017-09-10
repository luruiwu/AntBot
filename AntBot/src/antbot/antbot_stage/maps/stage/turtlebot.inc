define laser ranger
(
  sensor
  (
    range [0.05 10]
    fov 270.0
    samples 1080
  )
  # generic model properties
  color "yellow"
  size [ 0.1 0.1 0.1 ]
)
define block model
(
  size [0.4 0.400 0.100]
  gui_nose 0
)
define turtlebot position
(
  pose [ 0.0 0.0 0.0 0.0 ]
  odom_error [0.03 0.03 999999 999999 999999 0.02]

  size [ 1.3 0.8 0.40 ]
  origin [ 0.0 0.0 0.0 0.0 ]
  gui_nose 1
  drive "omni"
  color "gray"
  block(pose [0.7000 0.000 0.000 0.000 ] color "red")
  laser(pose [ 0.6 0.0 0.0 0.0 ])
)
