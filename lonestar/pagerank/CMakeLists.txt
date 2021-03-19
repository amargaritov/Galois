include_directories( ${PROJECT_SOURCE_DIR}/../../linux/dune )
link_directories( ${PROJECT_SOURCE_DIR}/../../linux/dune/libdune )

app(pagerank-pull PageRank-pull.cpp)
#app(pagerank-pull_dune PageRank-pull_dune.cpp)
app(pagerank-push PageRank-push.cpp)
