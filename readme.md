How to install
==============

The quick way
-------------

```
> git clone ssh://gitolite@ssh-codev-tuleap.cea.fr:2044/eos/EOS_code.git
> cd EOS_code
> ./configure --execdir=build/ --prefix=install/
> make -C build/
> make -C build/ test
> make -C build/ install
```

Going further
-------------

Here are some details about each installation step


Configuration
_____________

A good tip to use is to keep a file 'user_env.txt' outside the git repository and to use the configure option:

```
> ./configure --user-env-file=relative/or/absolute/path/to/user_env.txt --prefix=relative/or/absolute/path/ --execdir=relative/or/absolute/path/
```

If the --user-env-file option is not specified, then it is the user_env.txt of the root source directory which will be read.


If an option is specified several times, the last will override the other ones. Options given on the command line take precedence over the ones given in the user_env.txt.


Alternatively to the --execdir option, you can run the configure script from your build directory (which MUST be different thant the root source directory).

```
> mkdir my_build ; cd my_build     # Anywhere
> ${EOS_code_dir}/configure [--user-env-file=/path/to/user_env.txt [...]]
```

Or directly call CMake with all options on the command line:

```
> cmake ${EOS_code_dir} -B ${build_dir} \
      [-DCMAKE_INSTALL_PREFIX:PATH=/toto/install]\
      [-DUSER_MED_PATH:PATH=/path/to/med] [...]
```


Compilation
___________

If you are using a different build tool than Unix Makefile, you can run:

```
> cmake --build ${build_dir} [options]
```

Tests
_____

You can run ctest to launch parallel testing mode on N processes

```
> cd build/
> ctest . -j N
```

For CMake >= 3.20 users, you can run it outside the build directory:

```
> ctest --test-dir build/ -j N
```

Installation
____________

Here again, if you are using an other build tool, you can always run:

```
> cmake --build build/ [options] install
```


