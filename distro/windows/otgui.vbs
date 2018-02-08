dim fso: set fso = CreateObject("Scripting.FileSystemObject")
dim CurrentDirectory
CurrentDirectory = fso.GetAbsolutePathName(".")
Set wshShell = CreateObject("WScript.Shell")
Set wshEnv = wshShell.Environment("Process")
wshEnv("QT_PLUGIN_PATH") = fso.BuildPath(CurrentDirectory, "plugins")
wshShell.run fso.BuildPath(CurrentDirectory, "otgui.exe")
