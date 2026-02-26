============
Installation
============

Windows
-------
Persalys requires Windows >=10.

The default language setting can be overidden with the argument :code:`--lang=en|fr` added to the launcher executable command-line.

To do so right-click on the desktop shortcut, select ``Properties``, then edit the ``target`` field::

    ...\persalys_launcher.exe --lang en

In the same fashion the bundled software OpenGL renderer can be enabled/disabled with the argument :code:`--mesa=1|0`.

Linux
-----
Requirements:

  - glibc >=2.28 (Debian 10+, Ubuntu 18.10+, Fedora 29+, CentOS/RHEL 8+)
  - X11/Wayland with OpenGL >=3.2
  - FUSE userspace filesystem

The AppImage binary requires the libfuse2 package to run which might not be installed by default, for example on Ubuntu::

    $ sudo apt install libfuse2t64

No installation is required, set the executable bit if needed and just run the .AppImage as follows::

    $ chmod a+rx persalys-x.y-x86_64.AppImage
    $ ./persalys-x.y-x86_64.AppImage

The LANGUAGE environment variable can be set to ``en`` or ``fr`` to override the default language::

    $ LANGUAGE=fr ./persalys-x.y-x86_64.AppImage

Note that documentation links from the GUI cannot be opened in a snap-sandboxed browser: https://bugs.launchpad.net/snapd/+bug/1972762.

Additional Python packages
--------------------------

The Python environment can be amended with additional packages using Tools > Install a package

Modules are then installed in ``~/.persalys_base`` on Linux and in ``<USER_DIR>\AppData\Roaming\Persalys\`` on Windows.
