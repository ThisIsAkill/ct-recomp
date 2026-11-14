#!/usr/bin/env python3
"""tools/ctest_cache.py on a scratch git repo with a two-test CMake project:
the first run runs both and records them; a second run on the same tree
runs nothing and reports the same results (checks included); committing
the tree, or rewriting README.md / PROGRESS.md, keeps the records; editing
a source reruns; a failing test is rerun every time."""
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "tools"))
import ctest_cache  # noqa: E402

CMAKE = r'''cmake_minimum_required(VERSION 3.16)
project(t NONE)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
enable_testing()
add_test(NAME good COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/good.py ${CMAKE_BINARY_DIR})
add_test(NAME flaky COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/flaky.py ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})
'''


def main() -> int:
    fails = 0

    def check(what, got, want):
        nonlocal fails
        if got != want:
            print(f"FAIL {what}: got {got!r}, want {want!r}")
            fails += 1

    with tempfile.TemporaryDirectory() as tmp:
        src, build = os.path.join(tmp, "src"), os.path.join(tmp, "build")
        os.makedirs(src)
        git = lambda *a: subprocess.run(["git", "-c", "user.name=t", "-c", "user.email=t@t", *a],
                                        cwd=src, capture_output=True, check=True)
        git("init", "-q")
        with open(os.path.join(src, "CMakeLists.txt"), "w") as f:
            f.write(CMAKE)
        with open(os.path.join(src, "good.py"), "w") as f:
            f.write('import os, sys\nopen(os.path.join(sys.argv[1], "good.runs"), "a").write("run\\n")\nprint("good: 3 checks, 0 failed")\n')
        with open(os.path.join(src, "flaky.py"), "w") as f:
            f.write('import os, sys\nopen(os.path.join(sys.argv[1], "flaky.runs"), "a").write("run\\n")\nsys.exit(0 if os.path.exists(os.path.join(sys.argv[2], "ok")) else 1)\n')
        with open(os.path.join(src, ".gitignore"), "w") as f:
            f.write("build/\n")
        cfg = ["cmake", "-S", src, "-B", build]
        if shutil.which("ninja"):   # single-config: ctest needs no -C (VS default is multi-config)
            cfg += ["-G", "Ninja"]
        subprocess.run(cfg, capture_output=True, check=True)
        runs = lambda n: len(open(os.path.join(build, n + ".runs")).readlines())
        ctest_cache.ROOT = src

        res, ran = ctest_cache.cached_run(build)
        check("first run", (sorted(ran), res["good"], res["flaky"]["status"]),
              (["flaky", "good"], {"status": "Passed", "checks": 3}, "Failed"))
        res, ran = ctest_cache.cached_run(build)
        check("same tree: only the failure reruns", (ran, runs("good"), runs("flaky")),
              (["flaky"], 1, 2))
        check("reused result keeps its checks", res["good"], {"status": "Passed", "checks": 3})

        open(os.path.join(src, "ok"), "w").close()
        res, ran = ctest_cache.cached_run(build)
        check("new file: both rerun", (sorted(ran), res["flaky"]["status"]),
              (["flaky", "good"], "Passed"))
        git("add", "-A")
        git("commit", "-q", "-m", "x")
        for name in ("README.md", "PROGRESS.md"):
            with open(os.path.join(src, name), "w") as f:
                f.write("generated\n")
        _, ran = ctest_cache.cached_run(build)
        check("commit and generated files keep the records", ran, [])
        with open(os.path.join(src, "CMakeLists.txt"), "a") as f:
            f.write("# edit\n")
        _, ran = ctest_cache.cached_run(build)
        check("edited source reruns", sorted(ran), ["flaky", "good"])
    print("test_ctest_cache: " + ("ok" if not fails else f"{fails} failed"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
