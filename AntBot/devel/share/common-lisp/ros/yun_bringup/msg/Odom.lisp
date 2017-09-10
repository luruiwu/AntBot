; Auto-generated. Do not edit!


(cl:in-package yun_bringup-msg)


;//! \htmlinclude Odom.msg.html

(cl:defclass <Odom> (roslisp-msg-protocol:ros-message)
  ((vx
    :reader vx
    :initarg :vx
    :type cl:float
    :initform 0.0)
   (vy
    :reader vy
    :initarg :vy
    :type cl:float
    :initform 0.0)
   (vth
    :reader vth
    :initarg :vth
    :type cl:float
    :initform 0.0))
)

(cl:defclass Odom (<Odom>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Odom>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Odom)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name yun_bringup-msg:<Odom> is deprecated: use yun_bringup-msg:Odom instead.")))

(cl:ensure-generic-function 'vx-val :lambda-list '(m))
(cl:defmethod vx-val ((m <Odom>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader yun_bringup-msg:vx-val is deprecated.  Use yun_bringup-msg:vx instead.")
  (vx m))

(cl:ensure-generic-function 'vy-val :lambda-list '(m))
(cl:defmethod vy-val ((m <Odom>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader yun_bringup-msg:vy-val is deprecated.  Use yun_bringup-msg:vy instead.")
  (vy m))

(cl:ensure-generic-function 'vth-val :lambda-list '(m))
(cl:defmethod vth-val ((m <Odom>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader yun_bringup-msg:vth-val is deprecated.  Use yun_bringup-msg:vth instead.")
  (vth m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Odom>) ostream)
  "Serializes a message object of type '<Odom>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'vx))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'vy))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'vth))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Odom>) istream)
  "Deserializes a message object of type '<Odom>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'vx) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'vy) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'vth) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Odom>)))
  "Returns string type for a message object of type '<Odom>"
  "yun_bringup/Odom")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Odom)))
  "Returns string type for a message object of type 'Odom"
  "yun_bringup/Odom")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Odom>)))
  "Returns md5sum for a message object of type '<Odom>"
  "2ed912b0fd0de10da7b827f569b8b385")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Odom)))
  "Returns md5sum for a message object of type 'Odom"
  "2ed912b0fd0de10da7b827f569b8b385")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Odom>)))
  "Returns full string definition for message of type '<Odom>"
  (cl:format cl:nil "float32 vx~%float32 vy~%float32 vth~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Odom)))
  "Returns full string definition for message of type 'Odom"
  (cl:format cl:nil "float32 vx~%float32 vy~%float32 vth~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Odom>))
  (cl:+ 0
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Odom>))
  "Converts a ROS message object to a list"
  (cl:list 'Odom
    (cl:cons ':vx (vx msg))
    (cl:cons ':vy (vy msg))
    (cl:cons ':vth (vth msg))
))
