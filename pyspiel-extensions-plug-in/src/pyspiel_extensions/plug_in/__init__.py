import sys
import os

old_flags = sys.getdlopenflags()
sys.setdlopenflags(os.RTLD_GLOBAL | os.RTLD_LAZY)
import pyspiel
sys.setdlopenflags(old_flags)

import pyspiel_extensions.plug_in._core as extensions
