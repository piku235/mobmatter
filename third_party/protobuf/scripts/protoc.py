#!/usr/bin/env python3
import subprocess
import sys
import os

dir = os.path.dirname(__file__)

def main():
    result = subprocess.run([dir + "/protoc.sh"] + sys.argv[1:], check=True)
    return result.returncode

if __name__ == "__main__":
    sys.exit(main())
