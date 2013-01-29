=Utilities for working with UM output files=

* **stashcodes**: Prints a list of the stash code of each field in the file.
  Each time/height layer will produce its own code, to get a list of unique
  codes filter the output through `sort -n | uniq`
* **describefield**: Prints some information about a single stash code,
  including available times and height levels. Filter through `sort | uniq` to
  avoid repeats
* **uniqueheights**: Give every field with a given stash code a unique height
  level. This is useful for the histograms output by the COSP model, since
  tools like xconv assume unique heights for each field.
* **extractfield**: Create a netcdf file holding a single variable from a UM
  output, respecting pseudo levels

