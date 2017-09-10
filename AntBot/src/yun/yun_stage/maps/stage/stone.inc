

define stone model
(
  size [0.5 0.5 1.0]
  gui_nose 0
)

define stone position
(
  pose [ 0.0 0.0 0.0 0.0 ]
  odom_error [0.03 0.03 999999 999999 999999 0.02]
  block(pose [0.4000 0.000 0.000 0.000 ] color "red")
)
