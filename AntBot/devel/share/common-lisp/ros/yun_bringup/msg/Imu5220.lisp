; Auto-generated. Do not edit!


(cl:in-package yun_bringup-msg)


;//! \htmlinclude Imu5220.msg.html

(cl:defclass <Imu5220> (roslisp-msg-protocol:ros-message)
  ((angle
    :reader angle
    :initarg :angle
    :type cl:float
    :initform 0.0))
)

(cl:defclass Imu5220 (<Imu5220>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Imu5220>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Imu5220)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name yun_bringup-msg:<Imu5220> is deprecated: use yun_bringup-msg:Imu5220 instead.")))

(cl:ensure-generic-function 'angle-val :lambda-list '(m))
(cl:defmethod angle-val ((m <Imu5220>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader yun_bringup-msg:angle-val is deprecated.  Use yun_bringup-msg:angle instead.")
  (angle m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Imu5220>) ostream)
  "Serializes a message object of type '<Imu5220>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'angle))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Imu5220>) istream)
  "Deserializes a message object of type '<Imu5220>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'angle) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Imu5220>)))
  "Returns string type for a message object of type '<Imu5220>"
  "yun_bringup/Imu5220")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Imu5220)))
  "Returns string type for a message object of type 'Imu5220"
  "yun_bringup/Imu5220")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Imu5220>)))
  "Returns md5sum for a message object of type '<Imu5220>"
  "2d11dcdbe5a6f73dd324353dc52315ab")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Imu5220)))
  "Returns md5sum for a message object of type 'Imu5220"
  "2d11dcdbe5a6f73dd324353dc52315ab")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Imu5220>)))
  "Returns full string definition for message of type '<Imu5220>"
  (cl:format cl:nil "#角度Z~%float32 angle~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Imu5220)))
  "Returns full string definition for message of type 'Imu5220"
  (cl:format cl:nil "#角度Z~%float32 angle~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Imu5220>))
  (cl:+ 0
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Imu5220>))
  "Converts a ROS message object to a list"
  (cl:list 'Imu5220
    (cl:cons ':angle (angle msg))
))
