param([string] $type)
# NOTE: This file assumes you have ran some sort of VCVARS.
# in command prompt run vcvars, then after that run powershell from that command prompt.

if ($type -notmatch "((debug)|(release)){1}") {
	write-host release or debug please, $type will not do!
	exit;
}

$SOURCEFILES = $(get-childitem -filter "*.c??")
foreach ($i in $SOURCEFILES) {
	#to stop new-item from outputting to the console
	$suppress = new-item -force -ItemType file $("..\bin\obj\" + $i.toString().replace(".cpp", ".obj").replace(".c", ".obj"))
}
$OBJFILES = $(get-childitem ..\bin\obj -filter "*.obj")
$LIBFILES = "User32.lib Shell32.lib opengl32.lib Gdi32.lib SDL2main.lib SDL2.lib"

#All resource script files should be included bombed into res.rc
$hasrc = 0
if ($(get-childitem -filter "res.rc")) {
	$OBJFILES += " res.res"
	$hasrc = 1
}

$NAME = "Asteroids"

$CFLAGS += "/IC:\dev\include\ /c /Fo..\bin\obj\ /Fd..\bin\obj\ /EHsc /diagnostics:classic"
$LFLAGS += "/ENTRY:mainCRTStartup /OUT:../$NAME.exe"
$RFLAGS = "/fo..\bin\obj\res.res /v"
$WARNINGS = "/WX /wall"

if ($type -match "debug") {
	$CFLAGS += " /DDEBUG /Od /Zi /MDd"
	$LFLAGS += " /DEBUG /LIBPATH:C:\dev\lib\win64d\ /SUBSYSTEM:CONSOLE"
	$RFLAGS += " /dDEBUG"
	$WARNINGS += " "
}

if ($type -eq "release") {
	$CFLAGS += " /O2 /MT"
	$LFLAGS += " /LIBPATH:C:\dev\lib\win64\ /SUBSYSTEM:WINDOWS"
	$RFLAGS += " "
	$WARNINGS += " "
}

$clcommand = "cl " + $CFLAGS + " " + $SOURCEFILES
$linkcommand = "link " + $LFLAGS + " " + $OBJFILES + " " + $LIBFILES
$resourcecommand = "rc " + $RFLAGS + " res.rc"

write-host $NAME $type build
write-host $clcommand
write-host $linkcommand
if ($hasrc -eq 1) {
	write-host $resourcecommand
}
write-host "----"
write-host ""

invoke-expression $clcommand
if ($hasrc -eq 1) {
	invoke-expression $resourcecommand
}
push-location -path "..\bin\obj"
invoke-expression $linkcommand
pop-location
