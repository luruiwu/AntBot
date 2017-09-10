; Auto-generated. Do not edit!


(cl:in-package planner_msgs-msg)


;//! \htmlinclude GoToGoal.msg.html

(cl:defclass <GoToGoal> (roslisp-msg-protocol:ros-message)
  ((target
    :reader target
    :initarg :target
    :type (cl:vector planner_msgs-msg:goal)
   :initform (cl:make-array 0 :element-type 'planner_msgs-msg:goal :initial-element (cl:make-instance 'planner_msgs-msg:goal))))
)

(cl:defclass GoToGoal (<GoToGoal>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <GoToGoal>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'GoToGoal)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name planner_msgs-msg:<GoToGoal> is deprecated: use planner_msgs-msg:GoToGoal instead.")))

(cl:ensure-generic-function 'target-val :lambda-list '(m))
(cl:defmethod target-val ((m <GoToGoal>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader planner_msgs-msg:target-val is deprecated.  Use planner_msgs-msg:target instead.")
  (target m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <GoToGoal>) ostream)
  "Serializes a message object of type '<GoToGoal>"
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'target))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (roslisp-msg-protocol:serialize ele ostream))
   (cl:slot-value msg 'target))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <GoToGoal>) istream)
  "Deserializes a message object of type '<GoToGoal>"
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'target) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'target)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:aref vals i) (cl:make-instance 'planner_msgs-msg:goal))
  (roslisp-msg-protocol:deserialize (cl:aref vals i) istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<GoToGoal>)))
  "Returns string type for a message object of type '<GoToGoal>"
  "planner_msgs/GoToGoal")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'GoToGoal)))
  "Returns string type for a message object of type 'GoToGoal"
  "planner_msgs/GoToGoal")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<GoToGoal>)))
  "Returns md5sum for a message object of type '<GoToGoal>"
  "5c7eeba8aff53a215446e596e2937594")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'GoToGoal)))
  "Returns md5sum for a message object of type 'GoToGoal"
  "5c7eeba8aff53a215446e596e2937594")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<GoToGoal>)))
  "Returns full string definition for message of type '<GoToGoal>"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%# Define the goal~%goal[] target~%~%================================================================================~%MSG: planner_msgs/goal~%# Position ~%geometry_msgs/Pose2D pose~%# Desired speed~%float32 speed~%~%================================================================================~%MSG: geometry_msgs/Pose2D~%# This expresses a position and orientation on a 2D manifold.~%~%float64 x~%float64 y~%float64 theta~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'GoToGoal)))
  "Returns full string definition for message of type 'GoToGoal"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%# Define the goal~%goal[] target~%~%================================================================================~%MSG: planner_msgs/goal~%# Position ~%geometry_msgs/Pose2D pose~%# Desired speed~%float32 speed~%~%================================================================================~%MSG: geometry_msgs/Pose2D~%# This expresses a position and orientation on a 2D manifold.~%~%float64 x~%float64 y~%float64 theta~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <GoToGoal>))
  (cl:+ 0
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'target) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ (roslisp-msg-protocol:serialization-length ele))))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <GoToGoal>))
  "Converts a ROS message object to a list"
  (cl:list 'GoToGoal
    (cl:cons ':target (target msg))
))
