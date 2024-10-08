# USAGE
## Default options
### run example

```sh
mkdir build
cd build
cmake ..
make
./tree
```

 ### run tests
running tests from build directory
```sh
ctest
```

## Additional options

### build a tree graph

```sh
cmake -DPIC=ON ..
make
```

- dot file will be located in the main directory
- picture will be located in ```pic``` directory
