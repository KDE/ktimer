#! /bin/sh
$EXTRACTRC *.ui >> rc.cpp
$XGETTEXT *.cpp -o $podir/ktimer.pot
