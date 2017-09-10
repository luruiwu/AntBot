#!/bin/sh

if [ -n "$DESTDIR" ] ; then
    case $DESTDIR in
        /*) # ok
            ;;
        *)
            /bin/echo "DESTDIR argument must be absolute... "
            /bin/echo "otherwise python's distutils will bork things."
            exit 1
    esac
    DESTDIR_ARG="--root=$DESTDIR"
fi

echo_and_run() { echo "+ $@" ; "$@" ; }

echo_and_run cd "/home/pepper/AntBot/src/antbot/purepursuit_planner"

# snsure that Python install destination exists
echo_and_run mkdir -p "$DESTDIR/home/pepper/AntBot/install/lib/python2.7/dist-packages"

# Note that PYTHONPATH is pulled from the environment to support installing
# into one location when some dependencies were installed in another
# location, #123.
echo_and_run /usr/bin/env \
    PYTHONPATH="/home/pepper/AntBot/install/lib/python2.7/dist-packages:/home/pepper/AntBot/build/lib/python2.7/dist-packages:$PYTHONPATH" \
    CATKIN_BINARY_DIR="/home/pepper/AntBot/build" \
    "/usr/bin/python" \
    "/home/pepper/AntBot/src/antbot/purepursuit_planner/setup.py" \
    build --build-base "/home/pepper/AntBot/build/antbot/purepursuit_planner" \
    install \
    $DESTDIR_ARG \
    --install-layout=deb --prefix="/home/pepper/AntBot/install" --install-scripts="/home/pepper/AntBot/install/bin"
