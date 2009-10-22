#!/usr/bin/env bash

$EXTRACTRC *.ui >> rc.cpp
$XGETTEXT *.cpp -o $podir/kcm_touchpad.pot
rm -f rc.cpp
