
message("Connection Module.")

set(connection_name connection)

file (GLOB_RECURSE connection_all_file "${src_dir}/connection/*")
sourceGroup("${sourceGroupPrefixName}" ${connection_all_file})

