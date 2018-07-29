include_directories( ${PROJECT_SOURCE_DIR}/../../dune )
link_directories( ${PROJECT_SOURCE_DIR}/../../dune/libdune )

set(APP_EXTLIBS /home/ustiugov/dune/libdune/libdune.a)
app(pagerank-pull PageRank-pull.cpp)
app(pagerank-pull_dune PageRank-pull_dune.cpp)
app(pagerank-push PageRank-push.cpp)
