# CPCC

The competitive programmer's C++ translator.

## Usage

### Syntax

`use`s must be before all other code in CPC files:

```cpp
use macros;
use factorial;

int main() {
	...
}
```

will work, but

```cpp
#include <iostream>
use macros;
use factorial;

int main() {
	...
}
```

may cause unexpected behavior.

CPC is virtually identical to C++, except for having `use` libraries.

To use a library, simply put `use <libname>;` at the beginning of your code.

To compile your CPC program, run `cpcc file.cpc`.

If you would not like the credits comments at the beginning of your code, simplify run `cpcc --nocredits file.cpc`.

If you would not like warnings to be printed, simply run `cpcc --Wnone file.cpc`.

### Installing

As CPCC depends on its libraries, it is **highly recommended** that you build from source as opposed to installing a prebuilt version. You may need to install Boost for CPCC to work.

1. Clone the repository: 

```sh
git clone https://github.com/kevlu8/CPCC.git
```

2. Make a build folder

```sh
cd CPCC
mkdir build
cd build
```

3. Configure with CMake

```sh
cmake ..
```

4. Compile the source code

```sh
make -j4
```

5. Install the executable and libraries

```sh
sudo make install
```

You can now use CPCC! Run `cpcc --version` to check if it is installed properly.
