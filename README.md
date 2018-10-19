Obfuscate C/C++ programs by defining everything to invisible characters.

[Sample output](https://github.com/zhuowei/BleachThisCode/blob/master/sample/hello.out.c)

This uses Boost.Wave's lexer to extract all preprocessor tokens and replace them with `#define` preprocessor directives.

Inspired by [@willkirkby](https://twitter.com/willkirkby/status/1016014401479041024).

## Building

You need Boost.Wave and Meson.

## Running

`builddir/bleachthiscode input.c output.c`

The output can be compiled with Clang. (GCC doesn't like Unicode in source code.)

`clang output.c`

## License

MIT.
