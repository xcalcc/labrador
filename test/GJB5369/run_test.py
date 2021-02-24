#! /usr/bin/env python3
import os
import sys
import argparse
import subprocess
import coloredlogs, logging

# set up logger
logger = logging.getLogger(__name__)
coloredlogs.install(level='DEBUG', logger=logger)


def parse_dir(cur_dir):
    lst = os.listdir(cur_dir)
    res = []
    for i in lst:
        res.append(os.path.join(cur_dir, i))
    return res


def parse_file(cur_dir):
    subdirs = parse_dir(cur_dir)
    files = []
    for i in subdirs:
        tmp = parse_dir(os.path.join(i, "Src"))
        files += tmp
    return files


def make_masters(dirs):
    lst = []
    for i in dirs:
        sub_dir = parse_dir(i)
        lst += sub_dir

    for dir in lst:
        os.mkdir(os.path.join(dir, "Masters"))


def check_routine(cmd, file):
    path_lst = file.split("/")
    file_name = path_lst[-1].split(".")[0]
    master_dir = os.path.join(path_lst[0], path_lst[1], "Masters")
    stdout_file = os.path.join(master_dir, file_name + ".out")
    stderr_file = os.path.join(master_dir, file_name + ".err")

    cmd += " > " + stdout_file + " 2>" + stderr_file
    check_process = subprocess.run(cmd, shell=True,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)


def run_check(checker, items):
    cmd = checker + " -cc1 " + "-emit-llvm "
    for i in items:
        files = parse_file(i)
        # print(files)
        for file in files:
            tmp_cmd = cmd + file
            check_routine(tmp_cmd, file)
            exit(0)
        # check_process = subprocess.Popen()


def main(args):
    if args.make_masters:
        make_masters(args.m[0])

    if args.run_check:
        if args.checker_path:
            xsca_path = args.checker_path[0]
            if os.path.exists(xsca_path):
                if args.items:
                    items = args.items[0]
                    run_check(xsca_path, items)
                else:
                    logger.error("Set the ckeck items please.")
                    exit(1)
            else:
                logger.error("xsca not exists.")
                exit(1)
        else:
            logger.error("xsca path not set.")
            exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="run_test")

    parser.add_argument("-m", "--make-masters", action="append", type=str, nargs="+",
                        help=r'''
                        Make Master directories.
                        eg: ./run_test.py -m ./4.1 ./4.2
                        ''')

    parser.add_argument("-c", "--checker-path", type=str, nargs=1,
                        help="Set the checker position.")

    parser.add_argument("-r", "--run-check", action="store_true", default=False,
                        help="Run xsca check routines.")

    parser.add_argument("-i", "--items", action="append", type=str, nargs="+",
                        help=r'''
                        Set the test items.
                        eg: ./run_test.py -r -c ../../build/xsca -i 4.1 4.2
                        ''')

    parser.add_argument("-s", "--save-as-standard", action="store_true", default=False,
                        help="Save the checking result as a standard.")

    args = parser.parse_args()

    sys.exit(main(args))
