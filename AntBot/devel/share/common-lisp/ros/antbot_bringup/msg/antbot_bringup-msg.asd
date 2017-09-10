
(cl:in-package :asdf)

(defsystem "antbot_bringup-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "wheelSpeed" :depends-on ("_package_wheelSpeed"))
    (:file "_package_wheelSpeed" :depends-on ("_package"))
    (:file "antbotOdom" :depends-on ("_package_antbotOdom"))
    (:file "_package_antbotOdom" :depends-on ("_package"))
    (:file "antbot" :depends-on ("_package_antbot"))
    (:file "_package_antbot" :depends-on ("_package"))
    (:file "Num" :depends-on ("_package_Num"))
    (:file "_package_Num" :depends-on ("_package"))
  ))