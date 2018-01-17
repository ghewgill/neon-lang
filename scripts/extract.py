from __future__ import print_function

import sys
import tarfile
import zipfile

(archive, destination) = sys.argv[1:]

if archive.endswith(".tar.gz"):
    tarfile.open(archive).extractall(destination)
elif archive.endswith(".zip"):
    zipfile.ZipFile(archive).extractall(destination)
else:
    print("extract: unknown archive type for {}".format(archive), file=sys.stderr)
    sys.exit(1)
