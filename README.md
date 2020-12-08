# MCMC for Sampling Colorings

## Setup

Install the `igraph` C library and build the project by running `make all`. Our `Makefile` is configured
for compilation on a Mac machine where `igraph` has been installed via `homebrew`; you may have to adjust
`IGRAPH_INCLUDE_DIR` and `IGRAPH_LIB_DIR` in the `Makefile`.

## Usage

The executable `sample_colorings` will be contained in the `bin` directory after compilation. From the project
root directory, run e.g. `./bin/sample_colorings --num_vertices 8 --num_colors 5 --degree 3 --num_steps 1000`.

## Credits

This project was developed by Noah Singer (@singerng) and Grace Tian (@gracetian6) for Harvard's CS229r course, taught by Prof. Salil Vadhan.
