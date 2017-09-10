; Auto-generated. Do not edit!


(cl:in-package planner_msgs-msg)


;//! \htmlinclude goal.msg.html

(cl:defclass <goal> (roslisp-msg-protocol:ros-message)
  ((pose
    :reader pose
    :initarg :pose
    :type geometry_msgs-msg:Pose2D
    :initform (cl:make-instance 'geometry_msgs-msg:Pose2D))
   (speed
    :reader speed
    :initarg :speed
    :type cl:float
    :initform 0.0))
)

(cl:defclass goal (<goal>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <goal>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'goal)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name planner_msgs-msg:<goal> is deprecated: use planner_msgs-msg:goal instead.")))

(cl:ensure-generic-function 'pose-val :lambda-list '(m))
(cl:defmethod pose-val ((m <goal>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader planner_msgs-msg:pose-val is deprecated.  Use planner_msgs-msg:pose instead.")
  (pose m))

(cl:ensure-generic-function 'speed-val :lambda-list '(m))
(cl:defmethod speed-val ((m <goal>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader planner_msgs-msg:speed-val is deprecated.  Use planner_msgs-msg:speed instead.")
  (speed m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <goal>) ostream)
  "Serializes a message object of type '<goal>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'pose) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'speed))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <goal>) istream)
  "Deserializes a message object of type '<goal>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'pose) istream)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'speed) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<goal>)))
  "Returns string type for a message object of type '<goal>"
  "planner_msgs/goal")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'goal)))
  "Returns string type for a message object of type 'goal"
  "planner_msgs/goal")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<goal>)))
  "Returns md5sum for a message object of type '<goal>"
  "0e0e8dfef329b7a976daedf203ce03b5")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'goal)))
  "Returns md5sum for a message object of type 'goal"
  "0e0e8dfef329b7a976daedf203ce03b5")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<goal>)))
  "Returns full string definition for message of type '<goal>"
  (cl:format cl:nil "# Position ~%geometry_msgs/Pose2D pose~%# Desired speed~%float32 speed~%~%================================================================================~%MSG: geometry_msgs/Pose2D~%# This expresses a position and orientation on a 2D manifold.~%~%float64 x~%float64 y~%float64 theta~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'goal)))
  "Returns full string definition for message of type 'goal"
  (cl:format cl:nil "# Position ~%geometry_msgs/Pose2D pose~%# Desired speed~%float32 speed~%~%================================================================================~%MSG: geometry_msgs/Pose2D~%# This expresses a position and orientation on a 2D manifold.~%~%float64 x~%float64 y~%float64 theta~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <goal>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'pose))
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <goal>))
  "Converts a ROS message object to a list"
  (cl:list 'goal
    (cl:cons ':pose (pose msg))
    (cl:cons ':speed (speed msg))
))
