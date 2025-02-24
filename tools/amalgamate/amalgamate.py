#!/usr/bin/env python3

from __future__ import annotations

import logging
import re
from argparse import ArgumentParser
from pathlib import Path
from typing import Iterable


PROJECT = "piperifle"
VERSION = "0.0"
URL = "https://github.com/tylerh111/piperifle"

PATTERN_PRAGMA = re.compile(r"^\s*#\s*pragma\s+once")
PATTERN_INCLUDE = re.compile(r'^\s*#\s*include\s+(<|")(?P<path>.*)("|>)')

TRACE = logging.DEBUG - 1
logging.addLevelName(TRACE, "TRACE")

def _setup_logger(level: str):
    logging.basicConfig(
        level=level.upper(),
        format="{asctime} [{levelname:<8}] <{funcName}> {message}",
        style="{",
    )


def _preamble_replace(line: str):
    line = line.replace("@PROJECT@", PROJECT)
    line = line.replace("@VERSION@", VERSION)
    line = line.replace("@URL@", URL)
    return line


class Amalgamation:

    files: list[Path]
    outdirs: list[Path]
    incdirs: list[Path]
    ext: str
    preamble: list[str]

    def __init__(self, args):
        _setup_logger(args.log)

        self.files = args.files
        self.outdirs = args.outdir
        self.incdirs = args.incdir
        self.ext = args.ext
        self.preamble = []

        logging.debug(args)
        logging.info(f"files    = {[p.absolute().as_posix() for p in self.files]}")
        logging.info(f"outdirs  = {[p.absolute().as_posix() for p in self.outdirs]}")
        logging.info(f"incdirs  = {[p.absolute().as_posix() for p in self.incdirs]}")
        logging.info(f"preamble = {None if args.preamble is None else args.preamble.absolute().as_posix()}")

        preamble_file = args.preamble
        if preamble_file:
            with open(preamble_file, "r") as f:
                self.preamble = [_preamble_replace(line) for line in f]

        for d in self.outdirs:
            d.mkdir(exist_ok=True)

    def generate(self):
        for file in self.files:
            logging.info(f"reading from '{file}'")
            with file.open() as f:
                amalgamation = self._generate(f)

            if self.preamble:
                amalgamation = self.preamble + amalgamation

            for outdir in self.outdirs:
                if outdir:
                    outfile = outdir / file.name
                else:
                    outfile = file.with_suffix(".".join((file.suffix, self.ext)))

                logging.info(f"writing to   '{outfile}'")
                with outfile.open("w") as f:
                    f.writelines(amalgamation)

    def _generate(self, lines: Iterable[str]) -> list[str]:
        amalgamation = []
        for line in lines:
            pragma_match = PATTERN_PRAGMA.search(line)
            include_match = PATTERN_INCLUDE.search(line)

            # based on matches:
            # - comment out pragma once directives
            # - comment out expanded include directives
            # - otherwise add line to amalgamation
            if pragma_match:
                amalgamation.append(f"// {line}")
            elif include_match:
                logging.log(TRACE, f"searched {line.strip()}")
                logging.log(TRACE, f"{len(amalgamation)=}")
                include_path = Path(include_match.group("path"))
                include_path = self._search_include_dirs(include_path)
                if include_path:
                    amalgamation.append(f"// {line}")
                    with include_path.open() as f:
                        amalgamation.extend(self._generate(f))
                    amalgamation.append(f"// {line.strip()} (end)\n")
                else:
                    amalgamation.append(line)
            else:
                amalgamation.append(line)

        return amalgamation

    def _search_include_dirs(self, path: Path) -> Path | None:
        for incdir in self.incdirs:
            if (incdir / path).exists():
                return incdir / path

        return None


def main():
    parser = ArgumentParser(description="Amalgamate C/C++ files.")

    parser.add_argument(
        "files",
        type=Path,
        nargs="+",
        help="files to amalgamate",
    )

    parser.add_argument(
        "-O",
        "--outdir",
        action="extend",
        nargs=1,
        type=Path,
        default=[],
        help="output directories; default saves files next to source file",
    )

    parser.add_argument(
        "-I",
        "--incdir",
        action="extend",
        nargs=1,
        type=Path,
        default=[Path.cwd()],
        help="search include directories (default '.')",
    )

    parser.add_argument(
        "--ext",
        type=str,
        metavar="EXT",
        default="amalgamated",
        help=(
            "add extension to output file name (default 'amalgamated');"
            " only applicable if no output directory is specified"
        ),
    )

    parser.add_argument(
        "--preamble",
        type=Path,
        help="preamble code for the amalgamated file",
    )

    parser.add_argument(
        "-l",
        "--log",
        type=str,
        choices=["trace", "debug", "info", "warning", "error", "critical"],
        default="info",
        help="log level (default 'info')",
    )

    amalgamation = Amalgamation(parser.parse_args())
    amalgamation.generate()


if __name__ == "__main__":
    main()
