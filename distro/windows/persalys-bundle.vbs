dim fso: set fso = CreateObject("Scripting.FileSystemObject")
dim currentDirectory: currentDirectory = fso.GetParentFolderName(WScript.ScriptFullName)
Set wshShell = CreateObject("WScript.Shell")
Set wshEnv = wshShell.Environment("Process")

wshEnv("PYTHONPATH") = fso.GetAbsolutePathName(currentDirectory + "\..\..\..\python37.zip")

wshShell.Run "persalys.vbs"
