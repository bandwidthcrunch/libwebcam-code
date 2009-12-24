#!/bin/sh
rm -f doxygen/default.cfg &&
ln -s libwebcam.cfg doxygen/default.cfg &&
rm -rf doxygen-output &&
mkdir -p doxygen-output/libwebcam &&
make doxygen-doc &&
cp -v doxygen/res/* doxygen-output/libwebcam/html/
