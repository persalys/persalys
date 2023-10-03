# add Python directories
$env:PYTHONHOME = "$PSScriptRoot\..\..\.."
$env:PYTHONPATH = "$PSScriptRoot\..\..\..\python311.zip;$PSScriptRoot\..;$env:PYTHONPATH"
$env:PATH = "$PSScriptRoot\..\..\..;$PSScriptRoot\..\..\..\Scripts;$env:PATH"

# ParaView plugins
$env:PV_PLUGIN_PATH = "$PSScriptRoot\BagPlotViewsAndFilters"

# documentation path
$env:PERSALYS_HTML_DOCUMENTATION_PATH = "$PSScriptRoot\doc\html"

# https://github.com/pal1000/mesa-dist-win/blob/master/readme.md#opengl-context-configuration-override
$env:GALLIUM_DRIVER = "llvmpipe"

# allows to find the GL driver in the current directory: https://github.com/qt/qtbase/commit/a14f2d59
$env:QT_OPENGL_DLL = "opengl32"

# override default Qt5Agg backend (on conda) to avoid conflicts with newer MinGW Qt5 libs
$env:MPLBACKEND = "TkAgg"

# override default language setting
#$env:LANG = "en|fr"

Start-Process -FilePath "$PSScriptRoot\persalys.exe" -Wait
