#!/usr/bin/env bash
targets=(nuklear benchmark fmt xxhash libcuckoo skarupkeFHM gtest rapidxml luajit assimp gainput glfx glm il all)
argument="${1}"

function anyInstall {
    make install -C BUILD DESTDIR=../INSTALLDIR
    mkdir -p ../include/
    cp -rf INSTALLDIR/usr/local/include/* ../include/
    mkdir -p ../lib/
    cp -rf INSTALLDIR/usr/local/lib*/* ../lib/
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

function updateRapidXml {
    git clone https://github.com/discordapp/rapidxml
    mkdir -p include/rapidxml/
    mkdir -p lib/
	install -m 0644 rapidxml/rapidxml.hpp           include/rapidxml/
	install -m 0644 rapidxml/rapidxml_iterators.hpp include/rapidxml/
	install -m 0644 rapidxml/rapidxml_print.hpp     include/rapidxml/
	install -m 0644 rapidxml/rapidxml_utils.hpp     include/rapidxml/
	rm -rf rapidxml
}

function updateLuajit {
	git clone https://github.com/LuaJIT/LuaJIT
    pushd LuaJIT
	git checkout v2.1.0-beta3
	make
    mkdir -p ../lib/
    mkdir -p ../include/luajit-2.1/
	install -m 0755 src/libluajit.so ../lib/libluajit-5.1.so
	install -m 0755 src/libluajit.a ../lib/libluajit-5.1.a
	install -m 0644 src/lauxlib.h ../include/luajit-2.1/
	install -m 0644 src/lua.h ../include/luajit-2.1/
	install -m 0644 src/lua.hpp ../include/luajit-2.1/
	install -m 0644 src/luaconf.h ../include/luajit-2.1/
	install -m 0644 src/luajit.h ../include/luajit-2.1/
	install -m 0644 src/lualib.h ../include/luajit-2.1/
	ln -s ../lib/libluajit-5.1.so ../lib/libluajit-5.1.so.2
	popd
	rm -rf LuaJIT
}

function updateAssimp {
    git clone https://github.com/assimp/assimp
    pushd assimp
    git checkout v.5.0.0.rc1
    cmake . -BBUILD -DCMAKE_BUILD_TYPE=Release
    make -j2 -C BUILD
    anyInstall
    rm -rf assimp
}

function updateGainput {
    git clone https://github.com/jkuhlmann/gainput
    pushd gainput
    git checkout v1.0.0
    cmake . -BBUILD -DCMAKE_BUILD_TYPE=Release
    make -j2 -C BUILD
    anyInstall
    rm -rf gainput
}

function updateGlfx {
    git clone https://github.com/maizensh/glfx
    pushd glfx
    cmake . -BBUILD -DCMAKE_BUILD_TYPE=Release
    make -j2 -C BUILD
    anyInstall
    rm -rf glfx
}

function updateGlm {
    git clone https://github.com/g-truc/glm
    pushd glm
    git checkout 0.9.9.5
    cmake . -BBUILD -DCMAKE_BUILD_TYPE=Release
    make -j2 -C BUILD
    anyInstall
    rm -rf glm
}

function updateIl {
    git clone https://github.com/DentonW/DevIL
    pushd DevIL
    git checkout v1.8.0
    cmake DevIL -BBUILD -DCMAKE_BUILD_TYPE=Release
    make -j2 -C BUILD
    anyInstall
    rm -rf DevIL
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
rapidxml)    updateRapidXml    ;;
luajit)      updateLuajit      ;;
assimp)      updateAssimp      ;;
gainput)     updateGainput     ;;
glfx)        updateGlfx        ;;
glm)         updateGlm         ;;
il)          updateIl          ;;
all)
    rm -rf include
    rm -rf lib
    updateNuklear
    updateBenchmark
    updateFmt
    updateXxhash
    updateSkarupkeFHM
    updateGTest
    updateRapidXml
    updateLuajit
    updateAssimp
    updateGainput
    updateGlfx
    updateGlm
    updateIl
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
