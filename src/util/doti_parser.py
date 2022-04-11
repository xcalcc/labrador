#! /usr/bin/env python3
import re
import argparse
from typing import List, Tuple


class DotiParser:
    file: str
    forbidden_list: List[str]

    def __init__(self, file="", forbidden_list=str):
        self.file = file
        self.forbidden_list = forbidden_list.split(",")
        i = 0
        while i < len(self.forbidden_list):
            self.forbidden_list[i] = self.forbidden_list[i].replace(" ", "")
            if self.forbidden_list[i] == "":
                self.forbidden_list.remove("")
            i = i + 1

    def GetFileName(self, line: str) -> str:
        pattern = re.compile(r'".*"')
        results = pattern.findall(line)
        if len(results) == 0:
            return ""
        else:
            return results[0].strip('"')

    def GetLineNumber(self, line: str) -> str:
        pattern = re.compile(r'# \d+')
        results = pattern.findall(line)
        if len(results) == 0:
            return ""
        else:
            return results[0].strip("# ")

    def GetIncludeInfo(self, line: str) -> Tuple[str, str]:
        return self.GetFileName(line), self.GetLineNumber(line)

    # return format:
    # [('/usr/include/signal.h', 'a.c', '1'), ('/usr/include/stdio.h', 'a.c', '2')]
    def CollectHeaderFile(self, filename: str = "") -> List[Tuple[str, str, str]]:
        res = []
        if filename == "":
            filename = self.file
        with open(filename, "r") as f:
            lines = f.readlines()

            i = 1
            while i < len(lines):
                line = lines[i]
                cur_file, cur_line_num = self.GetIncludeInfo(line)
                if cur_line_num == "1" and (cur_file.split("/")[-1] in self.forbidden_list):

                    prev_line = lines[i - 1]
                    prev_file, prev_line_num = self.GetIncludeInfo(prev_line)
                    if prev_file != "" and prev_line_num != "":
                        include_info = (cur_file, prev_file, prev_line_num)
                        res.append(include_info)

                i = i + 1
                continue

            return res


if __name__ == "__main__":
    parser = argparse.ArgumentParser("DotI Parser")
    parser.add_argument("-f", "--file", type=str,
                        help="set input file name", action="store")
    parser.add_argument("-l", "--list", type=str,
                        help="search item list", action="store")
    args = parser.parse_args()

    doti = DotiParser(args.file, args.list)
    res = doti.CollectHeaderFile()
    print(res)
