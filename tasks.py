"""Invoke tasks for development.

Available tasks:
* building
* packaging
* testing
* formatting
* linting

**Notes**

To get a list of tasks, use `invoke -l`
Each task has a help menu (including invoke itself)
Use `invoke <task> --help` to get the help menu for a particular task.

It is recommended to use `invoke -e`.
This flag will echo the commands that are run to stdout in bold.

Invoke does not show color from commands that have colorful output.
Use `invoke -p` to run all commands in a pty which re-enables colorful output.
When commands are run in a pty, there is no distinction between stdout and stderr.

To debug tasks that are not working properly, use `invoke -d`.
This will show debgug statements happening during setup of invoke.
It will not set the debug logging inside the invoke tasks (this is hardcoded and must be manually overridden).

To see what commands would run without executing them, use `invoke -R`.
This is a dry run of the task and will show what commands would execute.
Note, commands that are dependent on outputs of other commands may not run.
"""

from __future__ import annotations

import logging
import os
import sys
from pathlib import Path
from typing import Callable

from invoke import Collection, Context, task


PROJECT = "piperifle"
VERSION = "0.0"

ROOT = Path(".")
BUILD = Path("build")
SOURCE = Path("src")
INSTALL = Path("/usr/local")
TOOLS = Path("tools")
DIST = Path("dist")

EXIT_CODE_SUCCESS = 0
EXIT_CODE_ERROR = 1

##==============================================================================
## setup
##==============================================================================

# setup logger
logging.basicConfig(
    level=logging.INFO,
    format="{asctime} [{levelname:<8}] <{funcName}> {message}",
    style="{",
)


##==============================================================================
## utilities
##==============================================================================

def _add_args_if(
    expr: bool,
    *args: str,
) -> tuple[()] | tuple[str, ...]:
    """Usage: `*_add_args_if(...)`"""
    return args if expr else ()

def _command(
    cmd: list[str] | Callable[[], list[str]],
    *args,
    sudo: bool = False,
    **kwargs,
) -> str:
    """Usage: `_command(<arg>, ...)` or `_command(<function>, <arg>, ...)`"""
    if isinstance(cmd, Callable):
        cmd = cmd(*args, **kwargs)
    if sudo:
        cmd = ["sudo", *cmd]
    return " ".join([str(arg) for arg in cmd])

def _strwrap(
    s: str,
    *,
    wrap: str = "'",
) -> str:
    """Usage: `_strwrap(<str>, <wrap>)` (<wrap> is any character, including chiral characters)"""
    l = r = wrap
    if len(wrap) == 2:
        l, r = wrap

    if wrap and wrap in "()": l, r = "()"
    if wrap and wrap in "[]": l, r = "[]"
    if wrap and wrap in "{}": l, r = "{}"
    if wrap and wrap in "<>": l, r = "<>"

    return f"{l}{s}{r}"


##==============================================================================
## tasks
##==============================================================================

@task(
    help={
        "devinfo": f"include development version information (requires git) (default)",
        "hidden": f"[plumbing] does not print version number",
    },
)
def version(
    c: Context,
    *,
    devinfo: bool = True,
    hidden: bool = False,
) -> str:
    """Piperifle version"""
    ver = VERSION

    def git_describe_cmd():
        return [
            "git",
            "describe",
            "--tags",
            "--dirty",
        ]  # fmt: skip

    # devinfo requires git to look up dynamic version information
    if devinfo:
        ver = c.run(_command(git_describe_cmd), hide="both").stdout.strip()

    if not hidden:
        print(f"{PROJECT} {ver}")

    return ver


@task
def format(c: Context):
    """(not implemented) Run formatter"""


@task
def lint(c: Context):
    """(not implemented) Run linter"""


@task(
    name="setup",
    help={
        "build": f"build directory (default: '{BUILD}')",
        "source": f"source directory (default: '{ROOT}')",
        "type": f"build type (options: plain, debug, debugoptimized, release, minsize) (default: unspecified)",
        "warnings": f"build warnings (options: 0, 1, 2, 3, everything) (default: unspecified)",
        "optimization": f"build optimization (options: plain, 0 , g, 1, 2, 3, s) (default: unspecified)",
        "reconfigure": f"reconfigure build (default)",
        "wipe": f"wipe build",
    }
)
def build_setup(
    c: Context,
    *,
    build: Path = BUILD,
    source: Path = ROOT,
    type: str | None = None,
    warnings: str | None = None,
    optimization: str | None = None,
    reconfigure: bool = True,
    wipe: bool = False,
):
    """Setup build"""

    build = Path(build)
    source = Path(source)

    type = type.lower() if isinstance(type, str) else None
    if type not in ("plain", "debug", "debugoptimized", "release", "minsize", None):
        logging.error(f"unknown build type: '{type}'")

    warnings = warnings.lower() if isinstance(warnings, str) else None
    if warnings not in ("0", "1", "2", "3", "everything", None):
        logging.error(f"unknown warning level: '{warnings}'")
        sys.exit(EXIT_CODE_ERROR)

    optimization = optimization.lower() if isinstance(optimization, str) else None
    if optimization not in ("plain", "0", "g", "1", "2", "3", "s", None):
        logging.error(f"unknown optimization level: '{optimization}'")
        sys.exit(EXIT_CODE_ERROR)

    def meson_setup_cmd():
        return [
            "meson",
            "setup",
            _strwrap(build),
            _strwrap(source),
            *_add_args_if(type, "--buildtype", type),
            *_add_args_if(warnings, "--warnlevel", warnings),
            *_add_args_if(optimization, "--optimization", optimization),
            *_add_args_if(reconfigure, "--reconfigure"),
            *_add_args_if(wipe, "--wipe"),
        ]  # fmt: skip

    c.run(_command(meson_setup_cmd))


@task(
    name="compile",
    optional=["jobs"],
    help={
        "build": f"build directory to build in (default: '{BUILD}')",
        "jobs": f"number of jobs to build with (default: 1)",
        "clean": f"clean build directory",
    },
)
def build_compile(
    c: Context,
    *,
    build: Path = BUILD,
    jobs: int | None = 1,
    clean: bool = False,
):
    """Compile code"""

    build = Path(build)
    jobs = None if jobs is True else jobs

    def meson_build_cmd():
        return [
            "meson",
            "compile",
            "-C", _strwrap(build),
            *_add_args_if(jobs is None, "-j"),
            *_add_args_if(jobs and jobs > 1, "-j", jobs),
            *_add_args_if(clean, "--clean"),
        ]  # fmt: skip

    c.run(_command(meson_build_cmd))


@task(
    name="install",
    help={
        "build": f"build directory (default: '{BUILD}')",
        "install": f"installation directory (default: unspecified)",
    },
)
def build_install(
    c: Context,
    *,
    build: Path = BUILD,
    install: Path | None = None,
):
    """Install build (may require sudo)"""

    build = Path(build)
    install = Path(install).absolute() if install else None

    def meson_install_cmd():
        return [
            "meson",
            "install",
            "-C", _strwrap(build),
            *_add_args_if(install, "--destdir", _strwrap(install)),
            "--no-rebuild",
        ]  # fmt: skip

    # requires sudo if directory does not exist or not writable
    # otherwise, let meson deside whether to use sudo
    if install and (not install.exists() or not os.access(install, os.W_OK)):
        c.run(_command(meson_install_cmd, sudo=True))
    else:
        c.run(_command(meson_install_cmd))


@task(
    name="dist",
    iterable=["format"],
    help={
        "build": f"build directory to create package (default: '{BUILD}')",
        "dirty": f"allow dirty repository",
        "formats": f"formats of distributions (options: xztar, bztar, gztar, zip) (default: unspecified)",
        "scratch": f"allow meson to run build and test (default)"
    },
)
def build_dist(
    c: Context,
    *,
    build: Path = BUILD,
    dirty: bool = False,
    formats: list[str] | None = None,
    scratch: bool = True,
):
    """Build distribution"""

    build = Path(build)
    formats = formats or []

    if any(format in ("xztar", "bztar", "gztar", "zip") for format in formats):
        logging.error(f"unknown format: '{formats}'")
        sys.exit(EXIT_CODE_ERROR)

    def meson_dist_cmd():
        return [
            "meson",
            "dist",
            "-C", _strwrap(build),
            *_add_args_if(dirty, "--allow-dirty"),
            *_add_args_if(formats, "--formats", _strwrap(",".join(formats))),
            *_add_args_if(not scratch, "--no-tests")
        ]  # fmt: skip

    c.run(_command(meson_dist_cmd))


@task(
    name="test",
    iterable=["tests", "testargs"],
    optional=["jobs"],
    help={
        "build": f"build directory (default: '{BUILD}')",
        "jobs": f"number of jobs to build with (default: 1)",
        "rebuild": f"[plumbing] rebuild before running tests",
        "suite": f"suite to run",
        "tests": f"tests to run",
        "testargs": f"test arguments",
    },
)
def build_test(
    c: Context,
    *,
    build: Path = BUILD,
    jobs: int | None = 1,
    rebuild: bool = True,
    suite: str | None = None,
    tests: list[str] | None = None,
    testargs: list[str] | None = None,
):
    """Run tests"""

    build = Path(build)
    jobs = None if jobs is True else jobs
    tests = tests or []
    testargs = testargs or []

    def meson_test_cmd():
        return [
            "meson",
            "test",
            "-C", _strwrap(build),
            *_add_args_if(jobs is None, "-j"),
            *_add_args_if(jobs and jobs > 1, "-j", jobs),
            *_add_args_if(not rebuild, "--no-rebuild"),
            *_add_args_if(suite, "--suite", _strwrap(suite)),
            *_add_args_if(testargs, "--test-args", _strwrap(",".join(testargs))),
            *_add_args_if(tests, *[_strwrap(test) for test in tests]),
        ]  # fmt: skip

    c.run(_command(meson_test_cmd))


@task(
    optional=["jobs"],
    help={
        "build": f"build directory (default: '{BUILD}')",
        "source": f"source directory (default: '{ROOT}')",
        "type": f"build type (options: plain, debug, debugoptimized, release, minsize) (default: unspecified)",
        "warnings": f"build warnings (options: 0, 1, 2, 3, everything) (default: unspecified)",
        "optimization": f"build optimization (options: plain, 0 , g, 1, 2, 3, s) (default: unspecified)",
        "reconfigure": f"reconfigure build (default)",
        "wipe": f"wipe build",
        "jobs": f"number of jobs to build with (default: 1)",
        "test": f"run tests (default)",
        "dist": f"build distribution (default)",
    },
)
def build(
    c: Context,
    *,
    build: Path = BUILD,
    source: Path = ROOT,
    type: str | None = None,
    warnings: str | None = None,
    optimization: str | None = None,
    reconfigure: bool = True,
    wipe: bool = False,
    jobs: int | None = 1,
    test: bool = True,
    dist: bool = True,
):
    """Setup, compile, package, and test code"""
    build_setup(
        c,
        build=build,
        source=source,
        type=type,
        warnings=warnings,
        optimization=optimization,
        reconfigure=reconfigure,
        wipe=wipe,
    )

    build_compile(
        c,
        build=build,
        jobs=jobs,
    )

    if test:
        build_test(
            c,
            build=build,
            jobs=jobs,
            rebuild=False,
        )

    if dist:
        build_dist(
            c,
            build=build,
            dirty=True,
            scratch=False,
        )


@task
def amalgamate(c: Context):

    def amalgamate_cmd():
        return [
            "python3",
            TOOLS / "amalgamate" / "amalgamate.py",
            "--preamble", TOOLS / "amalgamate" / "preamble.hpp",
            "-I", SOURCE,
            "-O", DIST,
            BUILD / SOURCE / "piperifle.hpp",
        ]  # fmt: skip

    c.run(_command(amalgamate_cmd))


@task
def docs_build(c: Context):
    """(not implemented) Build documentation"""


@task
def docs_serve(c: Context):
    """(not implemented) Server documentation"""


@task
def release_prepare(c: Context):
    """(not implemented) Prepare for release"""


##==============================================================================
## collections
##==============================================================================

ns_build = Collection("build")
ns_build.add_task(build, name="all", default=True)
ns_build.add_task(build_setup)
ns_build.add_task(build_compile)
ns_build.add_task(build_install)
ns_build.add_task(build_dist)
ns_build.add_task(build_test)

ns_docs = Collection("docs")
ns_docs.add_task(docs_build, name="build")
ns_docs.add_task(docs_serve, name="serve")

ns_release = Collection("release")
ns_release.add_task(release_prepare, name="prepare")

ns = Collection()
ns.add_collection(ns_build)
ns.add_collection(ns_docs)
ns.add_collection(ns_release)
ns.add_task(version)
ns.add_task(format)
ns.add_task(lint)
ns.add_task(amalgamate)
