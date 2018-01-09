# openmm-experiments

Conda environment for C++ development with [OpenMM][openmm].

[openmm]: http://openmm.org/

## Setting up the environment

Clone this repository and create the environment:

    git clone https://github.com/snsinfu/openmm-experiments.git
    cd openmm-experiments
    conda env create -f environment.yml

The last command creates a conda environment named `openmm-experiments`, in
which [the conda distribution of OpenMM][conda-openmm] is installed.

[conda-openmm]: https://anaconda.org/omnia/openmm

## Build and run an OpenMM program

Enter into the environment:

    source activate openmm-experiments

and now you can compile OpenMM programs. Example:

    c++ -std=c++14 smoke_test.cc -lOpenMM -Wl,-rpath,${CONDA_PREFIX}/lib
    ./a.out

The program should print "OK". The include path and the library location are
automatically set via `CPATH` and `LIBRARY_PATH` environment variables. Still,
you need to specify the `-Wl,-rpath,...` option to make the executable run
correctly.
