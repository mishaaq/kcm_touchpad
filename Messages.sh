#!/usr/bin/env bash
#lupdate -noobsolete -verbose *.ui -ts translations/kcm_touchpad_pl.ts

extractrc *.ui >> rc.cpp
xgettext rc.cpp -a -o po/kcm_touchpad.pot
rm -f rc.cpp
