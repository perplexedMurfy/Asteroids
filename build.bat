@echo off

rem so that we always execute in the same directory every time.

cd %~dp0

set code=components.cpp entity.cpp event.cpp keyboard.cpp main.cpp math.cpp RNG.cpp SDL_utils.cpp systems.cpp
set objs=components.obj entity.obj event.obj keyboard.obj main.obj math.obj RNG.obj SDL_utils.obj systems.obj
set libs=User32.lib Shell32.lib opengl32.lib Gdi32.lib SDL2main.lib SDL2.lib

if "%1"=="debug" (
    set copts=-IC:\dev\include\ -Od -Zi -MDd -DDEBUG
    set lopts=-ENTRY:mainCRTStartup -OUT:..\Asteroids.exe -LIBPATH:C:\dev\lib\win64\ -SUBSYSTEM:CONSOLE -DEBUG
) else (
    set copts=-IC:\dev\include\ -O2 -MT -WX 
    set lopts=-ENTRY:mainCRTStartup -OUT:..\Asteroids.exe -LIBPATH:C:\dev\lib\win64\ -SUBSYSTEM:WINDOWS
)

echo cl %copts% %code% -Fo..\bin\obj\ -c -EHsc
echo link %lopts% %objs% %libs%

pushd src
cl %copts% %code% -Fo..\bin\obj\ -c -EHsc
popd
pushd bin\obj
link %lopts% %objs% %libs%
popd
