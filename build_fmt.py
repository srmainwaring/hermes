#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import os
import sys
import shutil
import requests
import urllib
import zipfile
from subprocess import call
from distutils.spawn import find_executable

abs_path_root = os.getcwd()


# For parallel make
try:
    import multiprocessing
    nb_core = multiprocessing.cpu_count()
except ImportError:
    nb_core = 1


make = find_executable('make')
cmake = find_executable('cmake')
git = find_executable('git')
pip = find_executable('pip')


def build_cppfmt(build_type):
    print("\n\n==============================================================================")
    print("Building cpp-fmt library")
    print("==============================================================================")

    os.chdir('fmt')

    print """
    fmtlib
    """

    try:
        os.mkdir('build')
    except OSError:
        pass
    os.chdir('build')
    call([cmake, "..", "-DBUILD_SHARED_LIBS=TRUE", "-DFMT_TEST=FALSE"])
    call([make, "-j", str(nb_core)])

    os.chdir("../..")


def build(args):

    print("==============================================================================")
    print("Building thirdparty libraries for Hermes project")
    print("==============================================================================\n\n")

    if len(args) == 1:
        # By default, we build in Release mode
        build_type = "Debug"
    else:
        build_type = args[1]

        if (build_type == "clean"):
            clean()
            sys.exit(0)

        if build_type not in ("Debug", "Release", "RelWithDebInfo", "MinSizeRel"):
            raise NameError("Build type %s is not known. It has to be chosen among Debug, Release, RelWithDebInfo, MinSizeRel" % build_type)


    # Ensuring that thirdparty git submodules are up-to-date
    print("==============================================================================")
    print("Updating GIT SUBMODULES")
    print("==============================================================================")

    call([git, "submodule", "init"])
    call([git, "submodule", "update"])

    print("-- DONE.")

    build_cppfmt(build_type)


if __name__ == "__main__":

    build(sys.argv)
