
(cl:in-package :asdf)

(defsystem "yun_bringup-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "Odom" :depends-on ("_package_Odom"))
    (:file "_package_Odom" :depends-on ("_package"))
    (:file "Encoder" :depends-on ("_package_Encoder"))
    (:file "_package_Encoder" :depends-on ("_package"))
    (:file "Imu5220" :depends-on ("_package_Imu5220"))
    (:file "_package_Imu5220" :depends-on ("_package"))
    (:file "IO_Ctl" :depends-on ("_package_IO_Ctl"))
    (:file "_package_IO_Ctl" :depends-on ("_package"))
  ))