#!/bin/sh

pwd

windres.exe ../../../src/res/windows/$1_gui.rc -D$2 $3 -O coff -o $1.res

