; Auto-generated. Do not edit!


(cl:in-package planner_msgs-msg)


;//! \htmlinclude GoToResult.msg.html

(cl:defclass <GoToResult> (roslisp-msg-protocol:ros-message)
  ((route_result
    :reader route_result
    :initarg :route_result
    :type cl:integer
    :initform 0))
)

(cl:defclass GoToResult (<GoToResult>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <GoToResult>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'GoToResult)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name planner_msgs-msg:<GoToResult> is deprecated: use planner_msgs-msg:GoToResult instead.")))

(cl:ensure-generic-function 'route_result-val :lambda-list '(m))
(cl:defmethod route_result-val ((m <GoToResult>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader planner_msgs-msg:route_result-val is deprecated.  Use planner_msgs-msg:route_result instead.")
  (route_result m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <GoToResult>) ostream)
  "Serializes a message object of type '<GoToResult>"
  (cl:let* ((signed (cl:slot-value msg 'route_result)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <GoToResult>) istream)
  "Deserializes a message object of type '<GoToResult>"
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'route_result) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<GoToResult>)))
  "Returns string type for a message object of type '<GoToResult>"
  "planner_msgs/GoToResult")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'GoToResult)))
  "Returns string type for a message object of type 'GoToResult"
  "planner_msgs/GoToResult")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<GoToResult>)))
  "Returns md5sum for a message object of type '<GoToResult>"
  "69ac0a96c2c117c85b1bb21cd0b855f1")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'GoToResult)))
  "Returns md5sum for a message object of type 'GoToResult"
  "69ac0a96c2c117c85b1bb21cd0b855f1")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<GoToResult>)))
  "Returns full string definition for message of type '<GoToResult>"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%# Define the result~%int32 route_result~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'GoToResult)))
  "Returns full string definition for message of type 'GoToResult"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%# Define the result~%int32 route_result~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <GoToResult>))
  (cl:+ 0
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <GoToResult>))
  "Converts a ROS message object to a list"
  (cl:list 'GoToResult
    (cl:cons ':route_result (route_result msg))
))
