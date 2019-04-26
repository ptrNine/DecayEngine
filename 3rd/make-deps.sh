#!/usr/bin/env bash
targets=(nuklear benchmark fmt xxhash libcuckoo skarupkeFHM gtest all)
argument="${1}"

function anyInstall {
    make install -C BUILD DESTDIR=../INSTALL
    mkdir -p ../include/
    cp -rf INSTALL/usr/local/include/* ../include/
    mkdir -p ../lib/
    cp -rf INSTALL/usr/local/lib*/* ../lib/
    popd
}

function updateNuklear {
    git clone https://github.com/vurtun/nuklear
    mkdir -p include/
    cp nuklear/nuklear.h include/
    cp nuklear/demo/sdl_opengl2/nuklear_sdl_gl2.h include/
    rm -rf nuklear
}

function updateBenchmark {
    git clone https://github.com/google/benchmark
    pushd benchmark
    git checkout v1.4.1
    cmake . -BBUILD -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF
    make -j2 -C BUILD
    anyInstall
    rm -rf benchmark
}

function updateFmt {
    git clone https://github.com/fmtlib/fmt
    pushd fmt
    git checkout 5.3.0
    cmake . -BBUILD -DCMAKE_BUILD_TYPE=Release -DFMT_TEST=OFF -DFMT_INSTALL=ON -DBUILD_SHARED_LIBS=ON
    make -j2 -C BUILD
    anyInstall
    rm -rf fmt
}

function updateXxhash {
    git clone https://github.com/Cyan4973/xxHash
    pushd xxHash
    git checkout v0.7.0
    make -j2
    make install DESTDIR=INSTALL
    mkdir -p ../include/
    cp -rf INSTALL/usr/local/include/* ../include/
    mkdir -p ../lib/
    cp -rf INSTALL/usr/local/lib*/libxxhash.so* ../lib/
    popd
    rm -rf xxHash
}

function updateSkarupkeFHM {
    git clone https://github.com/skarupke/flat_hash_map
    mkdir -p include/
    cp -rf flat_hash_map/flat_hash_map.hpp include/
    rm -rf flat_hash_map
}

function updateGTest {
    git clone https://github.com/google/googletest
    pushd googletest
    git checkout release-1.8.1
    cmake . -BBUILD -DBUILD_GMOCK=ON -DINSTALL_GTEST=ON
    make -j2 -C BUILD
    anyInstall
    rm -rf googletest
}

function showHelp {
    echo "Usage: ./make-deps.sh [TARGET]"
    echo "Targets:"
    for i in "${targets[@]}"; do echo "    $i"; done
}


case "${argument}" in
nuklear)     updateNuklear     ;;
benchmark)   updateBenchmark   ;;
fmt)         updateFmt         ;;
xxhash)      updateXxhash      ;;
skarupkeFHM) updateSkarupkeFHM ;;
gtest)       updateGTest       ;;
all)
    rm -rf include
    rm -rf lib
    updateNuklear
    updateBenchmark
    updateFmt
    updateXxhash
    updateSkarupkeFHM
    updateGTest
;;
--help|-h)
    showHelp
;;
*)
    echo "Unknown target '${1}'"
    echo ""
    showHelp
;;
esac
