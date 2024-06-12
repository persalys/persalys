"""
Install class for installing pip modules in persalys environment
xref https://github.com/ronivaldo/nopip
"""

import os
import subprocess
import sys


class Install:
    @staticmethod
    def run_pip_command(command, verbose=True):
        python_exe = sys.executable
        if sys.platform.startswith("win"):
            python_exe = os.path.join(sys.prefix, "pythonw.exe")

        try:
            output = subprocess.run([python_exe, "-m", "pip"] + command + ["--no-cache-dir"],
                                    check=True, capture_output=verbose).stdout.decode()
        except subprocess.CalledProcessError as cpe:
            output = cpe.stdout.decode() + cpe.stderr.decode()
        if verbose:
            print(output)

    @staticmethod
    def module(package_name, verbose=True):
        Install.run_pip_command(["install", package_name], verbose=verbose)

    @staticmethod
    def modules(package_list, verbose=True):
        Install.run_pip_command(["install"] + package_list, verbose=verbose)


install = Install()
