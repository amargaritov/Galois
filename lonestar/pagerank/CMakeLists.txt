include_directories( /home/ustiugov/dune )
link_directories( /home/ustiugov/dune/libdune )

set(APP_EXTLIBS /home/ustiugov/dune/libdune/libdune.a)
app(pagerank-pull PageRank-pull.cpp)
app(pagerank-push PageRank-push.cpp)
