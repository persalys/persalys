dim fso: set fso = CreateObject("Scripting.FileSystemObject")
dim currentDirectory: currentDirectory = fso.GetParentFolderName(WScript.ScriptFullName)
Set wshShell = CreateObject("WScript.Shell")
Set wshEnv = wshShell.Environment("Process")

wshEnv("PYTHONHOME") = fso.GetAbsolutePathName(currentDirectory + "\..\..\..\")
wshEnv("PYTHONPATH") = fso.GetAbsolutePathName(currentDirectory + "\..\..\..\python310.zip")

wshShell.Run fso.GetAbsolutePathName(currentDirectory + "\persalys.vbs")
