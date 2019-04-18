"""
    otgui --- An OpenTURNS module
    ==================================

    Contents
    --------
      'otgui' is a module for OpenTURNS

"""

import sys
if sys.platform.startswith('win'):
    # this ensures OT dll is loaded
    import openturns

from .otguibase import *

__version__ = '7.0dev'
