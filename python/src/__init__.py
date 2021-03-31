"""
    Persalys --- An OpenTURNS module
    ==================================

    Contents
    --------
      'Persalys' is a module for OpenTURNS

"""

import sys
if sys.platform.startswith('win'):
    # this ensures OT dll is loaded
    import openturns

from .persalys import *

__version__ = '11.0dev'
