#!/usr/bin/env python
"""blur cli integration test"""
from __future__ import print_function
import subprocess as ps
import os
import sys


EXECUTABLE = os.path.join(os.path.dirname(__file__), "..", "build", "bin", "blur")
MASTER_PASSWD_FILE = "/tmp/master_passwd.stdin"
MASTER_PASSWD = "somemasterpassword"
TEST_RC_FILE = "/tmp/test.bg.bin"


def create_master_password_file():
    if os.path.exists(MASTER_PASSWD_FILE):
        os.remove(MASTER_PASSWD_FILE)
    if os.path.exists(TEST_RC_FILE):
        os.remove(TEST_RC_FILE)
    with open(MASTER_PASSWD_FILE, "w") as f:
        f.write(MASTER_PASSWD)


def __call_blur(arg_list, stdin):
    process = ps.Popen(arg_list, stdin=stdin, stdout=ps.PIPE, stderr=ps.PIPE)
    status = process.wait()
    return (status, *process.communicate())


def call_blur(*args):
    arg_list = [EXECUTABLE, "-s", "-n", "-f", TEST_RC_FILE]
    for arg in args:
        arg_list.append(arg)
    return __call_blur(arg_list, open(MASTER_PASSWD_FILE, "r"))


def main_test():
    for i in range(50):
        status, out, err = call_blur("add",
                                     "--name", "somepass" + str(i),
                                     "--description", "some description " + str(i),
                                     "--value", "somevalue" + str(i))

        if status != 0:
            return status, out, err

    for i in range(50 - 1, 0 - 1, -1):
        status, out, err = call_blur("get", "somepass" + str(i))

        if out.decode() != "somevalue" + str(i):
            sys.stderr.write("PASSWORDS DO NOT MATCH: " + str(out) + "\n")
            return 1

        if status != 0:
            return status

    return 0


if __name__ == "__main__":
    create_master_password_file()
    status = main_test()
    os.remove(MASTER_PASSWD_FILE)
    os.remove(TEST_RC_FILE)
    exit(status)
