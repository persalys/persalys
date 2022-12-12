dim fso: set fso = CreateObject("Scripting.FileSystemObject")
dim currentDirectory: currentDirectory = fso.GetParentFolderName(WScript.ScriptFullName)
Set wshShell = CreateObject("WScript.Shell")
Set wshEnv = wshShell.Environment("Process")

' override default Qt5Agg backend (on conda) to avoid conflicts with newer MinGW Qt5 libs
wshEnv("MPLBACKEND") = "TkAgg"

' add Python directory in Environment variable PATH
wshEnv("PATH") = fso.GetAbsolutePathName(currentDirectory + "\..\..\..") & ";" & wshEnv("PATH")
wshEnv("PATH") = fso.GetAbsolutePathName(currentDirectory + "\..\..\..\Scripts") & ";" & wshEnv("PATH")

' set PYTHONPATH
wshEnv("PYTHONPATH") = fso.GetAbsolutePathName(currentDirectory + "\..\") & ";" & wshEnv("PYTHONPATH")

' ParaView plugins
wshEnv("PV_PLUGIN_PATH") = fso.GetAbsolutePathName(currentDirectory + "\BagPlotViewsAndFilters")

' add documentation path in Environment variable PERSALYS_HTML_DOCUMENTATION_PATH
wshEnv("PERSALYS_HTML_DOCUMENTATION_PATH") = currentDirectory + "\doc\html\"

' https://github.com/pal1000/mesa-dist-win/blob/master/readme.md#opengl-context-configuration-override
wshEnv("GALLIUM_DRIVER") = "llvmpipe"

' allows to find the GL driver in the current directory: https://github.com/qt/qtbase/commit/a14f2d59
wshEnv("QT_OPENGL_DLL") = "opengl32"

' uncomment this to override default language: en/fr
' wshEnv("LANG") = "en"

wshShell.run chr(34) & fso.BuildPath(currentDirectory, "persalys.exe") & chr(34)
