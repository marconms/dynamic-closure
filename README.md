# Dynamic Closure

Dynamic Closure is a library for efficiently computing the closure of a changing graph.

#### Requirements

| Name      | Version |
| --------- | ------- |
| cmake     | ^3.5    |
| graphblas | ^7      |
| gcc       | ^13     |

To compile the library for release:

```zsh
mkdir ./Release && cd ./Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Once the library has been compiled, the binaries will be available in the `./Release/dynamic-closure` folder.

## Executing Benchmarks

Benchmarks are available in the `./Release/benchmarks` folder, once the library has been compiled for release.

Available benchmarks:

- generalized-floyd-warshall
- decrease-idempotent
- increase-column
- increase-row

Preloaded datasets:

- [Bai/rw5151](http://sparse.tamu.edu/Bai/rw5151)
- [Lucifora/cell1](http://sparse.tamu.edu/Lucifora/cell1)
- [Pajek/patents_main](http://sparse.tamu.edu/Pajek/patents_main)
- [vanHeukelum//cage10](http://sparse.tamu.edu/vanHeukelum/cage10)

To run all benchmarks, please execute the `./Release/benchmarks/run-benchmarks` binary. Benchmarks can also be executed indipendently.

#### Parameters

| Parameter | Description                                                                                                                                             | Required |
| --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------- | -------- |
| -f        | The path of the dataset to use.                                                                                                                         | Yes      |
| -o        | The path of the output directory.                                                                                                                       | No       |
| -n        | The number of updates to generate.                                                                                                                      | No       |
| -s        | A predefined closure to utilize for dynamic closure benchmarks. If this parameter is not specified, the closure will be computed before the benchmarks. | No       |

Examples (in the context of `./Release/benchmarks`):

```sh
./generalized-floyd-warshall -f ../assets/cage10.mtx -o ../out

./run-benchmarks -f ../assets/cage10.mtx -o ../out -s ../out/generalized-floyd-warshall_cage-10/F.mtx

./increase-column -f ../assets/cage10.mtx -o ../out -n 1000 -s ../out/generalized-floyd-warshall_cage-10/F.mtx

./increase-row -f ../assets/cage10.mtx -o ../out -n 1000 -s ../out/generalized-floyd-warshall_cage-10/F.mtx

./decrease-idempotent -f ../assets/cage10.mtx -o ../out -n 1000 -s ../out/generalized-floyd-warshall_cage-10/F.mtx
```
