CONDA_CPATH_BACKUP="${CPATH}"
CONDA_LIBRARY_PATH_BACKUP="${LIBRARY_PATH}"

export CPATH="${CONDA_PREFIX}/include:${CPATH}"
export LIBRARY_PATH="${CONDA_PREFIX}/lib:${LIBRARY_PATH}"
