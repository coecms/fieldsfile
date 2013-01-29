Utilities for working with UM output files
==========================================

* **stashcodes**: Prints a list of the stash code of each field in the file.
  Each time/height layer will produce its own code, to get a list of unique
  codes filter the output through `sort -n | uniq`
* **describefield**: Prints some information about a single stash code,
  including available times and height levels. Filter through `sort | uniq` to
  avoid repeats
* **extractfield**: Create a netcdf file holding a single variable from a UM
  output, respecting pseudo levels. Usage is `extractfield UMFILE STASH
  NETCDFFILE`, the netcdf file will be overwritten if it already exists

Building
--------

To build run `make` from the top directory. The code was built using the Intel
compiler, you may need to change the calls to _bswap64 to your compiler's byte
swapping intrinsic if using another compiler.

Netcdf is assumed to be in LD_LIBRARY_PATH, if not tell make where to find the
libary like:

    make CPPFLAGS+="-I/path/to/netcdf/include" LDFLAGS+="-L/path/to/netcdf/lib"

