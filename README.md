# Comparison of open_spiel / pyspiel extension methods.

The goal of this project is to investigate methods of extending **open_spiel** without modifying the original codebase,
  with possibility to run **pyspiel** in **editable mode**.

For testing, the following additions to open_spiel are implemented:
- `extensions/perft` - a sample algorithm;
- `extensions/perft_executable` - a standalone executeble;
- `extensions/subtraction_game` - a sample game.

Two extension methods are implemented:
- Linking with a shared library. **This method doesn't work properly.**
- Plug-in method, where pyspiel.so symbols are loaded into global namespace and found by dynamic loader at runtime.

Two more extension methods are discussed:
- Wrapping open_spiel's build process with custom CMakeLists.txt. In my project I abandoned this method in favor of the plug-in method so I don't provide implementation details here.
- Linking both pyspiel.so and the extension module dynamically against libopen_spiel.so.

## Prerequisites

Open Spiel is added to `third_party/open_spiel` as a submodule. Run

```
git submodule update --init --recursive
```

to fetch it. Create a virtual environment and activate it.

```
python -m venv .venv --prompt pyspiel_extensions
source .venv/bin/activate
pip install --upgrade pip
pip install --upgrade setuptools testresources
python3 -m pip install -r requirements.txt
```

Compile and install Open Spiel.
```
cd third_party/open_spiel
./install.sh
./open_spiel/scripts/build_and_run_tests.sh
```

You should see:
```
All tests passed. Nicely done!
```

Install into the virtual environment.
```
python -m pip install -v .
```

Go back to the root directory and verify that pyspiel can be imported.
```
cd ../..
python -c "import pyspiel; print(pyspiel.load_game('tic_tac_toe'))"
```

Your IDE likely uses compile_commands.json. To generate or update it, run:
```
mkdir build
cd build
cmake .. -G Ninja
ninja
cd ..
```

## Option A. `shared_library`: OpenSpiel as a C++ Library

Please note that this method **doesn't work** properly.

We follow the instructions from the [Using OpenSpiel as a C++ Library]().

```
cd third_party/open_spiel/build/
BUILD_SHARED_LIB=ON CXX=clang++ cmake -DPython3_EXECUTABLE=$(which python3) -DCMAKE_CXX_COMPILER=${CXX} ../open_spiel
make -j$(nproc) open_spiel
cd ../../../
```

`libopen_spiel.so` will be created in `third_party/open_spiel/build`.

Both extensions can be compiled with `libopen_spiel.so` with
```
python -m pip install -v -e shared_library/
```

If we import both `shared_library` **and** `pyspiel`, then they
  do not share the same symbol space.
E.g. they have separate game registry.

This works:
```
import pyspiel
from pyspiel_extensions import shared_library
game = shared_library.load_game("subtraction_game")
shared_library.backward_induction(game)
```

But this fails:
```
import pyspiel
from pyspiel_extensions import shared_library
pyspiel.load_game("subtraction_game")
```
(with error `pyspiel.SpielError: Unknown game 'subtraction_game'. Available games are: ...`)

In effect, we can't use open_spiel's python code with our extensions. See also `shared_library/tests/`
(you can run it with `python -m pytest`).

## Option B. `wrapper_package`: Compiling extensions within OpenSpiel.

This requires minimal changes to OpenSpiel's source code and it works.

```
python -m pip install -v -e wrapper_package
```

## Option C. `plug_in`: Compiling extensions alongside OpenSpiel.

## Option D. Reworking OpenSpiel's build process.
