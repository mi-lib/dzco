DZco - digital control library
=================================================================
Copyright (C) Tomomichi Sugihara (Zhidao) since 2000

-----------------------------------------------------------------
## [What is this?]

DZco is a library for digital control including:

- polynomial rational expression of transfer functions
- frequency domain analysis
- linear system (vector-matrix form)
- general linear system
- lag system
- PID controller
- miscellanies (adder, subtractor, limiter)
- digital filter (Butterworth filter, moving-average filter)
- function generators

ZEDA and ZM are required to be installed.

-----------------------------------------------------------------
## [Installation / Uninstallation]

### install

Install ZEDA and ZM in advance.

Move to a directly under which you want to install DZco, and run:

   ```
   % git clone https://github.com/zhidao/dzco.git
   % cd dzco
   ```

Edit **PREFIX** in *config* file if necessary in order to specify
a directory where the header files, the library and some utilities
are installed. (default: ~/usr)

   - header files: $PREFIX/include/dzco
   - library file: $PREFIX/lib
   - utilities: $PREFIX/bin

Then, make and install.

   ```
   % make && make install
   ```

### uninstall

Do:

   ```
   % make uninstall
   ```

which removes $PREFIX/lib/libdzco.so and $PREFIX/include/dzco.

-----------------------------------------------------------------
## [How to use]

When you want to compile your code *test.c*, for example, the following line will work.

   ```
   % gcc `dzco-config -L` `dzco-config -I` test.c `dzco-config -l`
   ```

-----------------------------------------------------------------
## [Contact]

zhidao@ieee.org
