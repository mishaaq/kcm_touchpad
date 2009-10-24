#!/usr/bin/env bash

extractrc *.ui >> rc.cpp
xgettext rc.cpp kcmtouchpad.cpp -o po/kcm_touchpad.pot --foreign-user -C -ki18n -ktr2i18n -kI18N_NOOP -kI18N_NOOP2
rm -f rc.cpp
