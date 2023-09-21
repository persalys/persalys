Persalys
========
OpenTURNS-based GUI for uncertainty treatment

Installation
------------
- Windows:

  * requirements: Windows>=10
  * the default language setting can be overidden with the argument --lang=en|fr added to the launcher executable command-line
  * The bundled software OpenGL renderer can be enabled/disabled with the argument --mesa=1|0

- Linux:

  * requirements: glibc>=2.17, X11/Wayland, OpenGL>=3.2, FUSE
  * no install required, just run the .AppImage
  * use non snap-sandboxed browser for doc links: https://bugs.launchpad.net/snapd/+bug/1972762

Environment variables:

- LANGUAGE: can be set to en|fr to override the language on Linux
- PERSALYS_HTML_DOCUMENTATION_PATH: override path to the html documentation
- PERSALYS_NO_GL: if defined, this disables OpenGL (used for ParaView widgets)
- PERSALYS_CALIBRATION_ENGINE: if defined to "adao" and adao support is enabled
  this switches to adao for calibration computations
