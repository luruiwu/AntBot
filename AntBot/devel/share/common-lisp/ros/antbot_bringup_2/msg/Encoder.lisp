; Auto-generated. Do not edit!


(cl:in-package antbot_bringup_2-msg)


;//! \htmlinclude Encoder.msg.html

(cl:defclass <Encoder> (roslisp-msg-protocol:ros-message)
  ((encoder
    :reader encoder
    :initarg :encoder
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0)))
)

(cl:defclass Encoder (<Encoder>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Encoder>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Encoder)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name antbot_bringup_2-msg:<Encoder> is deprecated: use antbot_bringup_2-msg:Encoder instead.")))

(cl:ensure-generic-function 'encoder-val :lambda-list '(m))
(cl:defmethod encoder-val ((m <Encoder>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader antbot_bringup_2-msg:encoder-val is deprecated.  Use antbot_bringup_2-msg:encoder instead.")
  (encoder m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Encoder>) ostream)
  "Serializes a message object of type '<Encoder>"
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'encoder))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'encoder))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Encoder>) istream)
  "Deserializes a message object of type '<Encoder>"
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'encoder) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'encoder)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Encoder>)))
  "Returns string type for a message object of type '<Encoder>"
  "antbot_bringup_2/Encoder")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Encoder)))
  "Returns string type for a message object of type 'Encoder"
  "antbot_bringup_2/Encoder")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Encoder>)))
  "Returns md5sum for a message object of type '<Encoder>"
  "f55857e2c2d7397d957a61ceadb6892a")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Encoder)))
  "Returns md5sum for a message object of type 'Encoder"
  "f55857e2c2d7397d957a61ceadb6892a")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Encoder>)))
  "Returns full string definition for message of type '<Encoder>"
  (cl:format cl:nil "#编码器的数据~%float32[] encoder~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Encoder)))
  "Returns full string definition for message of type 'Encoder"
  (cl:format cl:nil "#编码器的数据~%float32[] encoder~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Encoder>))
  (cl:+ 0
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'encoder) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Encoder>))
  "Converts a ROS message object to a list"
  (cl:list 'Encoder
    (cl:cons ':encoder (encoder msg))
))
