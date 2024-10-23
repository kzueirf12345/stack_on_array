# Stack

This static library allows you to reliably work with the stack, because canary and hash protections are implemented for this lib.
Work with the stack is also done through an encrypted key.

## Dependencies

### Make

Ubuntu:

```bash
sudo apt-get update
sudo apt install make
```
Windows:

[Guide](https://stackoverflow.com/questions/32127524/how-to-install-and-use-make-in-windows)

### gcc

Ubuntu:

```bash
sudo apt-get update
sudo apt install gcc
```

Windows:

[Guide](https://dev.to/gamegods3/how-to-install-gcc-in-windows-10-the-easier-way-422j)

## Usage

```bash
cd PATH/TO/DIR/stack/
make build
```
Then, you need to include ```libstack.h``` and link ```libstack.a```.

### Make commands
- build
- all
- clean
- clean_SMTH
- logger_SMTH

### Make variables
- **ADD_FLAGS** - your compilation flags will be added to the built-in ones
- **FLAGS** - you can redefine all compile flags
- **DEBUG_** - default value 1 (enable). Switch to 0, for disable

### Protect

To enable a specific protection you must pass the corresponding value during compilation through the -D flag. Also, you can enable test mode in a similar way.
- **-HASH_PROTECT**
- **-PENGUIN_PROTECT** (Canary protection)
- **-STACK_TEST_MODE**