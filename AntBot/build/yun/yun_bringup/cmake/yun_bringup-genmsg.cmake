# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "yun_bringup: 4 messages, 0 services")

set(MSG_I_FLAGS "-Iyun_bringup:/home/pepper/AntBot/src/yun/yun_bringup/msg;-Istd_msgs:/opt/ros/indigo/share/std_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(genlisp REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(yun_bringup_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Odom.msg" NAME_WE)
add_custom_target(_yun_bringup_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "yun_bringup" "/home/pepper/AntBot/src/yun/yun_bringup/msg/Odom.msg" ""
)

get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/IO_Ctl.msg" NAME_WE)
add_custom_target(_yun_bringup_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "yun_bringup" "/home/pepper/AntBot/src/yun/yun_bringup/msg/IO_Ctl.msg" ""
)

get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Imu5220.msg" NAME_WE)
add_custom_target(_yun_bringup_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "yun_bringup" "/home/pepper/AntBot/src/yun/yun_bringup/msg/Imu5220.msg" ""
)

get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Encoder.msg" NAME_WE)
add_custom_target(_yun_bringup_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "yun_bringup" "/home/pepper/AntBot/src/yun/yun_bringup/msg/Encoder.msg" ""
)

#
#  langs = gencpp;genlisp;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/Odom.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/yun_bringup
)
_generate_msg_cpp(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/IO_Ctl.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/yun_bringup
)
_generate_msg_cpp(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/Imu5220.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/yun_bringup
)
_generate_msg_cpp(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/Encoder.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/yun_bringup
)

### Generating Services

### Generating Module File
_generate_module_cpp(yun_bringup
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/yun_bringup
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(yun_bringup_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(yun_bringup_generate_messages yun_bringup_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Odom.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_cpp _yun_bringup_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/IO_Ctl.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_cpp _yun_bringup_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Imu5220.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_cpp _yun_bringup_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Encoder.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_cpp _yun_bringup_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(yun_bringup_gencpp)
add_dependencies(yun_bringup_gencpp yun_bringup_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS yun_bringup_generate_messages_cpp)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/Odom.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/yun_bringup
)
_generate_msg_lisp(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/IO_Ctl.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/yun_bringup
)
_generate_msg_lisp(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/Imu5220.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/yun_bringup
)
_generate_msg_lisp(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/Encoder.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/yun_bringup
)

### Generating Services

### Generating Module File
_generate_module_lisp(yun_bringup
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/yun_bringup
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(yun_bringup_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(yun_bringup_generate_messages yun_bringup_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Odom.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_lisp _yun_bringup_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/IO_Ctl.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_lisp _yun_bringup_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Imu5220.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_lisp _yun_bringup_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Encoder.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_lisp _yun_bringup_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(yun_bringup_genlisp)
add_dependencies(yun_bringup_genlisp yun_bringup_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS yun_bringup_generate_messages_lisp)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/Odom.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/yun_bringup
)
_generate_msg_py(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/IO_Ctl.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/yun_bringup
)
_generate_msg_py(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/Imu5220.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/yun_bringup
)
_generate_msg_py(yun_bringup
  "/home/pepper/AntBot/src/yun/yun_bringup/msg/Encoder.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/yun_bringup
)

### Generating Services

### Generating Module File
_generate_module_py(yun_bringup
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/yun_bringup
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(yun_bringup_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(yun_bringup_generate_messages yun_bringup_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Odom.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_py _yun_bringup_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/IO_Ctl.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_py _yun_bringup_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Imu5220.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_py _yun_bringup_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/pepper/AntBot/src/yun/yun_bringup/msg/Encoder.msg" NAME_WE)
add_dependencies(yun_bringup_generate_messages_py _yun_bringup_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(yun_bringup_genpy)
add_dependencies(yun_bringup_genpy yun_bringup_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS yun_bringup_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/yun_bringup)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/yun_bringup
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_cpp)
  add_dependencies(yun_bringup_generate_messages_cpp std_msgs_generate_messages_cpp)
endif()

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/yun_bringup)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/yun_bringup
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_lisp)
  add_dependencies(yun_bringup_generate_messages_lisp std_msgs_generate_messages_lisp)
endif()

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/yun_bringup)
  install(CODE "execute_process(COMMAND \"/usr/bin/python\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/yun_bringup\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/yun_bringup
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_py)
  add_dependencies(yun_bringup_generate_messages_py std_msgs_generate_messages_py)
endif()
