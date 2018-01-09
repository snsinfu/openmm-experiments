# openmm-experiments

Conda environment for C++ development with [OpenMM][openmm].

### Setting up environment

Clone this repository and run this command:

    conda env create -f environment.yml

This command creates a conda environment named `openmm-experiments`, in which
[the conda distribution of OpenMM][conda-openmm] is installed. You can test
the installation with the following commands:

    source activate openmm-experiments
    c++ -std=c++14 smoke_test.cc -lOpenMM -Wl,-rpath,${CONDA_PREFIX}/lib
    ./a.out

The program should print "OK".

[openmm]: http://openmm.org/
[conda-openmm]: https://anaconda.org/omnia/openmm
