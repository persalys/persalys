dim fso: set fso = CreateObject("Scripting.FileSystemObject")
dim currentDirectory: currentDirectory = fso.GetParentFolderName(WScript.ScriptFullName)
Set wshShell = CreateObject("WScript.Shell")
Set wshEnv = wshShell.Environment("Process")

rem override default Qt5Agg backend (on conda) to avoid conflicts with newer MinGW Qt5 libs
wshEnv("MPLBACKEND") = "TkAgg"

rem add Python directory in Environment variable PATH
wshEnv("PATH") = wshEnv("PATH") & ";" & fso.GetAbsolutePathName(currentDirectory + "\..\..\..")

rem set PYTHONPATH
wshEnv("PYTHONPATH") = fso.GetAbsolutePathName(currentDirectory + "\..\") & ";" & wshEnv("PYTHONPATH")

rem ParaView plugins
wshEnv("PV_PLUGIN_PATH") = fso.GetAbsolutePathName(currentDirectory + "\BagPlotViewsAndFilters")

rem add documentation path in Environment variable PERSALYS_HTML_DOCUMENTATION_PATH
wshEnv("PERSALYS_HTML_DOCUMENTATION_PATH") = currentDirectory + "\doc\html\"

rem https://github.com/pal1000/mesa-dist-win/blob/master/readme.md#opengl-context-configuration-override
wshEnv("MESA_GL_VERSION_OVERRIDE") = "3.2"

rem uncomment this to override default language: en/fr
rem wshEnv("LANG") = "en"

rem need quotes if contains spaces (C:\Program Files)
wshShell.run Chr(34) & fso.BuildPath(currentDirectory, "persalys.exe") & Chr(34)
