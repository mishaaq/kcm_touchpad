#!/usr/bin/env bash
#lupdate -noobsolete -verbose *.ui -ts translations/kcm_touchpad_pl.ts

$EXTRACTRC *.ui >> rc.cpp
$XGETTEXT *.cpp -o $podir/kcm_touchpad.pot
rm -f rc.cpp
