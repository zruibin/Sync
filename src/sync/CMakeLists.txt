
message("Sync Module.")

set(sync_name sync)

file(GLOB_RECURSE sync_all_file "${src_dir}/sync/*")
sourceGroup("${sourceGroupPrefixName}" ${sync_all_file})

