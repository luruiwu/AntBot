; Auto-generated. Do not edit!


(cl:in-package yun_bringup-msg)


;//! \htmlinclude IO_Ctl.msg.html

(cl:defclass <IO_Ctl> (roslisp-msg-protocol:ros-message)
  ((led
    :reader led
    :initarg :led
    :type cl:fixnum
    :initform 0)
   (fa
    :reader fa
    :initarg :fa
    :type cl:fixnum
    :initform 0)
   (charge
    :reader charge
    :initarg :charge
    :type cl:fixnum
    :initform 0))
)

(cl:defclass IO_Ctl (<IO_Ctl>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <IO_Ctl>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'IO_Ctl)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name yun_bringup-msg:<IO_Ctl> is deprecated: use yun_bringup-msg:IO_Ctl instead.")))

(cl:ensure-generic-function 'led-val :lambda-list '(m))
(cl:defmethod led-val ((m <IO_Ctl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader yun_bringup-msg:led-val is deprecated.  Use yun_bringup-msg:led instead.")
  (led m))

(cl:ensure-generic-function 'fa-val :lambda-list '(m))
(cl:defmethod fa-val ((m <IO_Ctl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader yun_bringup-msg:fa-val is deprecated.  Use yun_bringup-msg:fa instead.")
  (fa m))

(cl:ensure-generic-function 'charge-val :lambda-list '(m))
(cl:defmethod charge-val ((m <IO_Ctl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader yun_bringup-msg:charge-val is deprecated.  Use yun_bringup-msg:charge instead.")
  (charge m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <IO_Ctl>) ostream)
  "Serializes a message object of type '<IO_Ctl>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'led)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'fa)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'charge)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <IO_Ctl>) istream)
  "Deserializes a message object of type '<IO_Ctl>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'led)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'fa)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'charge)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<IO_Ctl>)))
  "Returns string type for a message object of type '<IO_Ctl>"
  "yun_bringup/IO_Ctl")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'IO_Ctl)))
  "Returns string type for a message object of type 'IO_Ctl"
  "yun_bringup/IO_Ctl")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<IO_Ctl>)))
  "Returns md5sum for a message object of type '<IO_Ctl>"
  "c09423b168532f99ab3ae02d90d808ec")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'IO_Ctl)))
  "Returns md5sum for a message object of type 'IO_Ctl"
  "c09423b168532f99ab3ae02d90d808ec")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<IO_Ctl>)))
  "Returns full string definition for message of type '<IO_Ctl>"
  (cl:format cl:nil "uint8 led #0-2~%uint8 fa  #0-2~%uint8 charge #0-2~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'IO_Ctl)))
  "Returns full string definition for message of type 'IO_Ctl"
  (cl:format cl:nil "uint8 led #0-2~%uint8 fa  #0-2~%uint8 charge #0-2~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <IO_Ctl>))
  (cl:+ 0
     1
     1
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <IO_Ctl>))
  "Converts a ROS message object to a list"
  (cl:list 'IO_Ctl
    (cl:cons ':led (led msg))
    (cl:cons ':fa (fa msg))
    (cl:cons ':charge (charge msg))
))
