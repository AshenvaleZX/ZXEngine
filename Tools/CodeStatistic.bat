set targetpath=..\CPPScripts\
for /f %%a in ('dir /b /a-d %targetpath%*.cpp, %targetpath%*.h') do lua LinesCount.lua %targetpath% %%a %~dp0

set targetpath=..\CPPScripts\Editor\
for /f %%a in ('dir /b /a-d %targetpath%*.cpp, %targetpath%*.h') do lua LinesCount.lua %targetpath% %%a %~dp0

set targetpath=..\CPPScripts\LuaWrap\
for /f %%a in ('dir /b /a-d %targetpath%*.cpp, %targetpath%*.h') do lua LinesCount.lua %targetpath% %%a %~dp0

set targetpath=..\CPPScripts\Math\
for /f %%a in ('dir /b /a-d %targetpath%*.cpp, %targetpath%*.h') do lua LinesCount.lua %targetpath% %%a %~dp0

lua Add.lua %~dp0
del tmp.txt
pause