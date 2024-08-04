
message("MD5")

set(md5_name md5)

file(GLOB_RECURSE md5_all_file "${libs_dir}/md5/*")
sourceGroup("" ${md5_all_file})
