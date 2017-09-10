
(cl:in-package :asdf)

(defsystem "antbot_bringup_2-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "Encoder" :depends-on ("_package_Encoder"))
    (:file "_package_Encoder" :depends-on ("_package"))
    (:file "Imu5220" :depends-on ("_package_Imu5220"))
    (:file "_package_Imu5220" :depends-on ("_package"))
  ))