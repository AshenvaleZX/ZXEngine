@echo off
setlocal enabledelayedexpansion

set "path_to_folder=..\CPPScripts"
set "file_types=(.cpp .h)"
set "exclude_path1=..\CPPScripts\Lua"
set "exclude_path2=..\CPPScripts\ImGui"
set total_lines=0

pushd "%exclude_path1%"
set "abs_exclude_path1=%CD%"
popd

pushd "%exclude_path2%"
set "abs_exclude_path2=%CD%"
popd

for /R "%path_to_folder%" %%F in (*.cpp *.h) do (
    set "current_path=%%~dpF"
    if not "!current_path!"=="!abs_exclude_path1!\" if not "!current_path!"=="!abs_exclude_path2!\" (
        set current_file_lines=0
        for /f "tokens=1,* delims=:" %%A in ('findstr /n "^" "%%F"') do (
            set /a current_file_lines+=1
            set /a total_lines+=1
        )
        echo Lines in %%F: !current_file_lines!
    )
)

echo Total lines of cpp code: %total_lines%

:end
pause