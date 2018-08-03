include_directories( ${PROJECT_SOURCE_DIR}/../../dune )
link_directories( ${PROJECT_SOURCE_DIR}/../../dune/libdune )

app(bfs bfs.cpp)
app(bfs_dune bfs_dune.cpp)

add_test_scale(web bfs "${BASEINPUT}/random/r4-2e26.gr")
add_test_scale(small bfs "${BASEINPUT}/structured/rome99.gr")
