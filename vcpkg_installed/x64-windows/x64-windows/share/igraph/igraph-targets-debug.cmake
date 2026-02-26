#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "igraph::igraph" for configuration "Debug"
set_property(TARGET igraph::igraph APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(igraph::igraph PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/igraph.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/igraph.dll"
  )

list(APPEND _cmake_import_check_targets igraph::igraph )
list(APPEND _cmake_import_check_files_for_igraph::igraph "${_IMPORT_PREFIX}/debug/lib/igraph.lib" "${_IMPORT_PREFIX}/debug/bin/igraph.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
