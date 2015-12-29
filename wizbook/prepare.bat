if !%1 == ! goto usage
if !%2 == ! goto usage
if not exist %1 goto usage
wizcomp %1 %2
if errorlevel 1 goto end
tomemo %2 1005 wb %2
goto end
:usage
echo prepare inputfilaname outputname
echo   The outputname should have no extension.
echo   You can then upload outputname to your Organizer.
goto end
:end
