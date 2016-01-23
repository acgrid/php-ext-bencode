# PHP Extension for Bencode Encoding and Decoding
This PHP7 extension is a pure C implementation for bencode routine, which ported from [this](https://github.com/acgrid/php-bencode-extension) by unknown original author.

The orginal extension name and function names are kept at this moment.

## Installation

*SUPPORT PHP7 ONLY*, no external libraries are needed.

### General
```bash
phpize
./configure
make
make test
sudo make install
```

### Windows
Follow [this instruction](https://wiki.php.net/internals/windows/stepbystepbuild) for preparing a complete building environment for PHP. It is worthy to spend some time on.

I have done this with VS2015 community edition.

Then checkout the repo to your `php-sdk\phpdev\vc14\x64\pecl\` (you should mkdir `pecl` manually, in the same level of the php sources like 
`php-7.0.2-src`.

Open the VS2015 x64 native prompt (Do not follow the instruction above except that you need just a x86 build.) and do the same things
till run the batch file `bin\phpsdk_setvars.bat`. 

Change cwd to `php-sdk\phpdev\vc14\x64\pecl\php-7.x.y-src` during developing and building from now on.

- Run `buildconf` for a `configure.bat` with all pecl stuff included
- Run `configure --disable-all --disable-zts --enable-cli --enable-cg_bcode=shared` for a minimal PHP build to save time. Remove `--disable-zts` for ZTS build.
- Run `nmake` to start building
- Run `nmake test TESTS=../pecl/php-ext-bencode/tests/*.phpt` for testing the extension only (optional)
- Run `nmake build-dist` for packing everything useful up (official) or just copy the generated DLL under `x64\Release` (may differs according to arch, debug and ZTS)

## Usage

### `bencode()`

Very simple with recognition of PHP variable types. No additional type specifitions are need to create a bencode element.

Dictionary keys are sorted automatically without affecting input array.

```php
bencode([]); // le
bencode(new StdClass); // de
bencode(['b' => 1, 'a' => 2]); // d1:ai2e1:bi1ee
```

### `bdecode()`

The type information is mapped to PHP type as well.

Note that `de` and `le` are both converted to `[]` at this moment.

```php
bencode('i1e'); // int(1)
bdecode('li2e3:123i0ee'); // [2, '123', 0]
bdecode('d1:a3:1231:bi3e1:cl1:x1:y1:zee'); // ['a' => '123', 'b' => 3, 'c' => ['x', 'y', 'z']]
```

## Bencode syntax standard

The syntax for testing and QA is described [here](https://wiki.theory.org/BitTorrentSpecification#Bencoding).

It support converting unlimited integer-like string to bencoded like `i10000000000000e`.

It *does not* support parsing becoded dict with non-string key like `di0e3:fooe`.

## Discuss

- Rewrite the decoder with scanner `re2c` and parser `bison`, which PHP officially like ext/json
- Globally last error reporting like ext/json and return null silently on failure

This extension is aimed to boost performance for client making use of everything of the bencoded data, or exporting a structure in bencode. It do not provide extra functionality.

It consumes memory to some extent because maps all data to userspace, which is not suit for some tasks like editing `resume.dat`, when only part of data is needed to be read and written. You can try [this extension](https://github.com/Frederick888/php-bencode) for such applications.

## Todos

- Encode string qualified to be an integer as `int`
- INI setting to raise error in exception or warning
- INI setting/function parameter to specify decode `dict` as object

## License
PHP License
