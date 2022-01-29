rem @echo off

set app=mbs1
set path=%path%;"C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE";C:\Windows\Microsoft.NET\Framework\v4.0.30319

if not "%1"=="" set tag=/t:%1

svn update

cd source\VC++2010

rem msbuild.exe %app%_agar.vcxproj %tag% /p:Configuration=ReleaseAgar;Platform=Win32
rem msbuild.exe %app%_agar.vcxproj %tag% /p:Configuration=ReleaseAgar;Platform=x64

msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release_Win;Platform=Win32
msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release_Win;Platform=x64

msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release2_Win;Platform=Win32
msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release2_Win;Platform=x64

rem msbuild.exe %app%_agar.vcxproj %tag% /p:Configuration=ReleaseAgar_Dbgr;Platform=Win32
rem msbuild.exe %app%_agar.vcxproj %tag% /p:Configuration=ReleaseAgar_Dbgr;Platform=x64

msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release_Win_Dbgr;Platform=Win32
msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release_Win_Dbgr;Platform=x64

msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release2_Win_Dbgr;Platform=Win32
msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release2_Win_Dbgr;Platform=x64

msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release_Win_Z80B_Dbgr;Platform=Win32
msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release_Win_Z80B_Dbgr;Platform=x64

msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release2_Win_Z80B_Dbgr;Platform=Win32
msbuild.exe %app%_win.vcxproj %tag% /p:Configuration=Release2_Win_Z80B_Dbgr;Platform=x64

cd ..\..
