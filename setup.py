#!/usr/bin/env python
from __future__ import print_function
import os, sys, tempfile

try:
    import urllib.request as urllib2
except ImportError:
    import urllib2

try:
    import shutil

    def cmdpath(path):
        return shutil.which(path)

except ImportError:

    def cmdpath(cmd, path=None):
        if path is None:
            path = os.environ["PATH"].split(os.pathsep)

        for prefix in path:
            filename = os.path.join(prefix, cmd)
            executable = os.access(filename, os.X_OK)
            is_not_directory = os.path.isfile(filename)
            if executable and is_not_directory:
                return filename
        return None


def run_system(cmd):
    print("$", cmd)
    return os.system(cmd)

if "darwin" in sys.platform:
    PLATFORM = "darwin"
elif "cygwin" in sys.platform or "win" in sys.platform:
    PLATFORM = "win"
elif "linux" in sys.platform:
    PLATFORM = "linux"
else:
    PLATFORM = "unknown"

LIB_GLFW_PREDEFINED = {
    "darwin": [
        "-framework IOKit",
        "-framework OpenGL",
        "-framework Cocoa",
        "-lglfw3",
        "-Wl,-w",
    ],
    "win": ["-lglfw3"],
    "linux": ["-lglfw3"],
}


def download(dst, url):
    if os.path.exists(dst):
        print("Reusing {0}.".format(dst))
        return
    print("Downloading {0}...".format(dst))
    web = urllib2.urlopen(urllib2.Request(url, headers={"User-Agent": "Mozilla/5.0"}))
    with open(dst, "wb") as f:
        f.writelines(web.readlines())
    print("Done.".format(dst))


def check_includes(includes, path=None, lang="c++"):
    src_fd, src_fname = tempfile.mkstemp()
    for include in includes:
        os.write(src_fd, "#include <{0}>\n".format(include).encode("utf-8"))
    os.close(src_fd)
    return run_system("clang -fsyntax-only -x{0} {1}".format(lang, src_fname))


def check_libraries(libraries, path=[], name=None, lang="c"):
    print(f"checking: {libraries=}, {path=}, {name=}, {lang=}")
    src_fd, src_fname = tempfile.mkstemp()
    os.write(src_fd, b"int main() {}")
    os.close(src_fd)
    out_fd, out_fname = tempfile.mkstemp()
    os.close(out_fd)
    args = ""

    for library in libraries:
        args += library

    if len(path) == 0:
        path_str = ""
    else:
        path_str = "{0} ".format(" ".join(map(lambda l: "-L'{0}'".format(l), path)))

    if lang == "c++":
        compiler = "clang++"
    else:
        compiler = "clang"

    res = run_system(
        "{5} -x{4} {0} -o {1} {2}{3}".format(
            src_fname, out_fname, path_str, args, lang, compiler
        )
    )

    if res != 0:
        print("No libraries on default path {0}found.".format("for {0} ".format(name)))
        print(
            "You can (0) add a library search path, (1) change the linker flags or (2) do both."
        )
        manual = input("[0-2] > ").strip()
        if manual == "":
            print("No option selected!")
            return None
        try:
            opt = int(manual)
            if not (0 <= opt <= 2):
                print("Value must be between 0 and 2 (inclusive)!")
                return None

            if opt == 0 or opt == 2:
                new_path = input(
                    "Change the library search path, use colon to separate multiple (currently: '{0}'):\n> ".format(
                        ":".join(path)
                    )
                ).strip()
                path = new_path.split(":")

            if opt == 1 or opt == 2:
                new_libraries = input(
                    "Change the linker flags (currently: '{0}'):\n> ".format(
                        " ".join(libraries)
                    )
                ).strip()
                libraries = new_libraries.split()
        except ValueError:
            print("Must be a number!")
            return None

        return check_libraries(libraries, path=path, name=name, lang=lang)

    if path_str == "": return libraries
    return [path_str] + libraries


print("Checking for required tools...")

print("- ninja")
if cmdpath("ninja") is None:
    print(
        "Executable `ninja` not found in path!\nFor more info: https://ninja-build.org/"
    )
    exit(1)

print("- clang")
if cmdpath("clang") is None:
    print(
        "Executable `clang` not found in path!\nFor more info: https://clang.llvm.org/"
    )
    exit(1)

print("- clang++")
if cmdpath("clang++") is None:
    print(
        "Executable `clang++` not found in path!\nFor more info: https://clang.llvm.org/"
    )
    exit(1)

print("- glm")
if (
    check_includes(
        ["glm/glm.hpp", "glm/gtx/quaternion.hpp", "glm/gtc/type_ptr.hpp"], lang="c++"
    )
    != 0
):
    print(
        "Failed to compile test glm program. Most likely the glm headers are not present."
    )
    print("For more info: https://glm.g-truc.net/")
    exit(1)

print("- glfw")

if check_includes(["GLFW/glfw3.h"], lang="c") != 0:
    print(
        "Failed to compile test GLFW program. Most likely the GLFW headers are not present."
    )
    print("For more info: https://glfw.org/")
    exit(1)

if PLATFORM in LIB_GLFW_PREDEFINED:
    lib_glfw = LIB_GLFW_PREDEFINED[PLATFORM]
else:
    print("Could not identify platform!")
    flags = input("You can enter GLFW linker flags manually:\n> ").strip()
    if flags == "":
        print("Warning: No flags entered!")
    lib_glfw = flags.split()

glfw_res = check_libraries(lib_glfw, lang="c", name="GLFW")
if glfw_res is None:
    print(
        "Failed to link test GLFW program. Most likely the GLFW libraries are not present."
    )
    print("For more info: https://glfw.org/")
    exit(1)
else:
    lib_glfw = glfw_res

print("All required tools found!")
print("Setting up environment...")
print("- gl3w")
os.chdir("gtamfx")
run_system(sys.executable + " gl3w.py")
os.chdir("..")
print("- stb_image.h")
download(
    "gtamfx/include/stb_image.h",
    "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h",
)
print("- ninja")
with open("gtamfx/platform.ninja", "w") as f:
    f.write("platform_cflags = {0}\n".format(""))
    f.write("platform_ldflags = {0}\n".format(" ".join(lib_glfw)))
print("Environment setup!")
