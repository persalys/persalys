Persalys
========
OpenTURNS-based GUI for uncertainty treatment

Installation
------------
- Windows:

  * requirements: Windows>=10
  * to use system OpenGL libs remove the provided opengl32.dll

- Linux:

  * requirements: glibc>=2.17, X11/Wayland, OpenGL>=3.2, FUSE
  * no install required, just run the .AppImage

Environment variables:

- LANG: can be set to en/fr to override the language on Windows, see persalys.vbs
- LANGUAGE: same on Linux
- PERSALYS_HTML_DOCUMENTATION_PATH: override path to the html documentation
- PERSALYS_NO_GL: if defined, this disables OpenGL (used for ParaView widgets)
- PERSALYS_CALIBRATION_ENGINE: if defined to "adao" and adao support is enabled
  this switches to adao for calibration computations
