dim fso: set fso = CreateObject("Scripting.FileSystemObject")
dim CurrentDirectory
CurrentDirectory = fso.GetAbsolutePathName(".")
Set wshShell = CreateObject("WScript.Shell")
Set wshEnv = wshShell.Environment("Process")

rem override default Qt5Agg backend (on conda) to avoid conflicts with newer MinGW Qt5 libs
wshEnv("MPLBACKEND") = "TkAgg"

rem add Python directory in Environment variable PATH
WshEnv("PATH") = WshEnv("PATH") & ";" + fso.GetAbsolutePathName(CurrentDirectory + "\..\..\..")

wshShell.run fso.BuildPath(CurrentDirectory, "otgui.exe")
