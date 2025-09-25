import os
from invoke import task


@task
def install_custom_pyspiel(c):
    """Install custom pyspiel with specific build configuration."""

    # 1. Apply the patch to third_party/open_spiel
    print("Applying patch to third_party/open_spiel...")
    c.run("git apply --directory=third_party/open_spiel pyspiel-extensions-plug-in/setup.py.git.patch")

    # 2. Change directory to third_party/open_spiel
    os.chdir("third_party/open_spiel")

    try:
        # 3. Create build directory
        print("Creating build directory...")
        c.run("mkdir -p build/")

        # Change to build directory for cmake
        with c.cd("build"):
            # 4. Run cmake with specific configuration
            print("Running cmake configuration...")
            cmake_cmd = (
                "cmake -DPython3_EXECUTABLE=$(which python3) "
                "-DCMAKE_CXX_COMPILER=/usr/bin/clang++ "
                "-DBUILD_TYPE=RelWithDebInfo "
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=1 "
                "-DCMAKE_CXX_FLAGS=\"-fvisibility=default\" "
                "-DCMAKE_MODULE_LINKER_FLAGS=\"-Wl,--export-dynamic -Wl,--whole-archive -Wl,--allow-multiple-definition\" "
                "-DCMAKE_CXX_VISIBILITY_PRESET=default "
                "-DCMAKE_VISIBILITY_INLINES_HIDDEN=OFF "
                "-DCMAKE_POSITION_INDEPENDENT_CODE=ON "
                "../open_spiel -G Ninja"
            )
            c.run(cmake_cmd)

            # 5. Run ninja to build pyspiel
            print("Building pyspiel with ninja...")
            c.run("ninja -v pyspiel")

            # 6. Check if build/python/pyspiel.so was generated
            pyspiel_so_path = "build/python/pyspiel.so"
            if not os.path.exists(pyspiel_so_path):
                raise Exception(f"Failed to generate {pyspiel_so_path}")

            # 7. Set PYSPIEL_SO_PATH to full path
            full_pyspiel_so_path = os.path.abspath(pyspiel_so_path)
            os.environ['PYSPIEL_SO_PATH'] = full_pyspiel_so_path
            print(f"PYSPIEL_SO_PATH set to: {full_pyspiel_so_path}")

        # 8. Install with pip
        print("Installing with pip...")
        c.run("pip install --no-build-isolation -v .", env={'PYSPIEL_SO_PATH': full_pyspiel_so_path})

    finally:
        # 9. Change directory back to project root
        # We need to go back 2 levels: from third_party/open_spiel to project root
        project_root = os.path.abspath("../..")
        os.chdir(project_root)

        # 10. Reverse the patch
        print("Reversing the patch...")
        c.run("git apply -R --directory=third_party/open_spiel pyspiel-extensions-plug-in/setup.py.git.patch")

    print("Custom pyspiel installation completed successfully!")
