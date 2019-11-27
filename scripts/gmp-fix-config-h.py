#!/usr/bin/env python3

import sys

for s in sys.stdin:
    print(s.replace("#undef WANT_TMP_ALLOCA", "#define WANT_TMP_ALLOCA 1"))
