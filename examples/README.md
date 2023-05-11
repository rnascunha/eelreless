# Examples
How to run the examples.

At the root of the project (dictory above this) run the following commands:
```bash
# menuconfig
$ idf.py menuconfig -C <examples/folder> -DEXTRA_COMPONENT_DIRS=$PWD/components

# build
$ idf.py build -C <examples/folder> -DEXTRA_COMPONENT_DIRS=$PWD/components

# flash and monitor
$ idf.py flash monitor -C <examples/folder>

# clean
$ idf.py clean -C <examples/folder>

# fullclean
$ idf.py fullclean -C <examples/folder>
```
Where `<examples/folder>` is the path to the example (e. g `examples/http/login_wifi`).

To build all examples:
```bash
$ bash examples/run.sh .
```