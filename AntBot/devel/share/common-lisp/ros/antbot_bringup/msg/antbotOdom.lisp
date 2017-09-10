; Auto-generated. Do not edit!


(cl:in-package antbot_bringup-msg)


;//! \htmlinclude antbotOdom.msg.html

(cl:defclass <antbotOdom> (roslisp-msg-protocol:ros-message)
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

(cl:defclass antbotOdom (<antbotOdom>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <antbotOdom>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'antbotOdom)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name antbot_bringup-msg:<antbotOdom> is deprecated: use antbot_bringup-msg:antbotOdom instead.")))

(cl:ensure-generic-function 'vx-val :lambda-list '(m))
(cl:defmethod vx-val ((m <antbotOdom>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader antbot_bringup-msg:vx-val is deprecated.  Use antbot_bringup-msg:vx instead.")
  (vx m))

(cl:ensure-generic-function 'vy-val :lambda-list '(m))
(cl:defmethod vy-val ((m <antbotOdom>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader antbot_bringup-msg:vy-val is deprecated.  Use antbot_bringup-msg:vy instead.")
  (vy m))

(cl:ensure-generic-function 'vth-val :lambda-list '(m))
(cl:defmethod vth-val ((m <antbotOdom>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader antbot_bringup-msg:vth-val is deprecated.  Use antbot_bringup-msg:vth instead.")
  (vth m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <antbotOdom>) ostream)
  "Serializes a message object of type '<antbotOdom>"
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
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <antbotOdom>) istream)
  "Deserializes a message object of type '<antbotOdom>"
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
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<antbotOdom>)))
  "Returns string type for a message object of type '<antbotOdom>"
  "antbot_bringup/antbotOdom")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'antbotOdom)))
  "Returns string type for a message object of type 'antbotOdom"
  "antbot_bringup/antbotOdom")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<antbotOdom>)))
  "Returns md5sum for a message object of type '<antbotOdom>"
  "2ed912b0fd0de10da7b827f569b8b385")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'antbotOdom)))
  "Returns md5sum for a message object of type 'antbotOdom"
  "2ed912b0fd0de10da7b827f569b8b385")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<antbotOdom>)))
  "Returns full string definition for message of type '<antbotOdom>"
  (cl:format cl:nil "float32 vx~%float32 vy~%float32 vth~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'antbotOdom)))
  "Returns full string definition for message of type 'antbotOdom"
  (cl:format cl:nil "float32 vx~%float32 vy~%float32 vth~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <antbotOdom>))
  (cl:+ 0
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <antbotOdom>))
  "Converts a ROS message object to a list"
  (cl:list 'antbotOdom
    (cl:cons ':vx (vx msg))
    (cl:cons ':vy (vy msg))
    (cl:cons ':vth (vth msg))
))
