@echo off
if exist %1.pdb goto PalmFont
if exist %1.data goto MacFont
if exist %1.wzf goto JustWzf
if exist %1.txt goto TextFont
echo Font %1 not located.
goto end
:PalmFont
font2wiz %1
goto JustWzf
:MacFont
font2wiz %1 r m
goto JustWzf
:TextFont
txt2wzf %1
:JustWzf
tomemo -e %1 999 fn %1.wzf
:end
