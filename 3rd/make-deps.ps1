$targets  = @("nuklear","benchmark","fmt","xxhash","libcuckoo","skarupkeFHM","gtest","rapidxml","luajit","all")
$argument = $Args[0]

function anyInstall {
    mingw32-make install -C BUILD DESTDIR=../INSTALL
    New-Item -ItemType directory -Force -Path ..\include\
    New-Item -ItemType directory -Force -Path ..\lib\
	Push-Location INSTALL\
	Set-Location (Get-ChildItem)[0]
	Set-Location (Get-ChildItem)[0]
	Copy-Item -Recurse -Force include\* -Destination ..\..\..\..\include\
	Copy-Item -Recurse -Force lib\* -Destination ..\..\..\..\lib\
    Pop-Location
    Pop-Location
}

function updateNuklear {
    git clone https://github.com/vurtun/nuklear
    New-Item -ItemType directory -Force -Path include\
    Copy-Item -Force nuklear\nuklear.h -Destination include\
    Copy-Item -Force nuklear\demo\sdl_opengl2\nuklear_sdl_gl2.h -Destination include
    Remove-Item -Recurse -Force nuklear
}

function updateBenchmark {
    git clone https://github.com/google/benchmark
    Push-Location benchmark
    git checkout v1.4.1
    cmake . -BBUILD -G "MinGW Makefiles" -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF
	mingw32-make
	anyInstall
	Remove-Item -Recurse -Force benchmark
}

function updateFmt {
    git clone https://github.com/fmtlib/fmt
    Push-Location fmt
    git checkout 5.3.0
    cmake . -BBUILD -G "MinGW Makefiles" -DFMT_TEST=OFF -DFMT_INSTALL=ON -DBUILD_SHARED_LIBS=ON
    mingw32-make
	anyInstall
    Remove-Item -Recurse -Force fmt
}

function updateXxhash {
    git clone https://github.com/Cyan4973/xxHash
    Push-Location xxHash
    git checkout v0.7.0
	Push-Location cmake_unofficial
    cmake . -B..\BUILD -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=ON
    Pop-Location
	mingw32-make
    mingw32-make install -C BUILD DESTDIR=../INSTALL
    New-Item -ItemType directory -Force -Path ..\include\
    New-Item -ItemType directory -Force -Path ..\lib\
	Push-Location INSTALL\
	Set-Location (Get-ChildItem)[0]
	Set-Location (Get-ChildItem)[0]
	Copy-Item -Recurse -Force include\* -Destination ..\..\..\..\include\
	Copy-Item -Recurse -Force lib\* -Destination ..\..\..\..\lib\
    Pop-Location
	Copy-Item -Recurse -Force BUILD\libxxhash.dll -Destination ..\lib\
    Pop-Location
	Remove-Item -Recurse -Force xxHash
}

function updateSkarupkeFHM {
    git clone https://github.com/skarupke/flat_hash_map
    New-Item -ItemType directory -Force -Path include\
    Copy-Item -Recurse -Force flat_hash_map/flat_hash_map.hpp -Destination include/
    Remove-Item -Recurse -Force flat_hash_map
}

function updateGTest {
    git clone https://github.com/google/googletest
    Push-Location googletest
    git checkout release-1.8.1
    cmake . -BBUILD -G "MinGW Makefiles" -DBUILD_GMOCK=ON -DINSTALL_GTEST=ON
    anyInstall
	Remove-Item -Recurse -Force googletest
}

function updateRapidXml {
    git clone https://github.com/discordapp/rapidxml
	New-Item -ItemType directory -Force -Path include\rapidxml
	Copy-Item -Force rapidxml\rapidxml.hpp -Destination include\rapidxml
	Copy-Item -Force rapidxml\rapidxml_iterators.hpp -Destination include\rapidxml
	Copy-Item -Force rapidxml\rapidxml_print.hpp -Destination include\rapidxml
	Copy-Item -Force rapidxml\rapidxml_utils.hpp -Destination include\rapidxml
	Remove-Item -Recurse -Force rapidxml
}

function updateLuajit {
	git clone https://github.com/LuaJIT/LuaJIT
    Push-Location LuaJIT
	git checkout v2.1.0-beta3
	mingw32-make
    New-Item -ItemType directory -Force -Path ..\include\luajit-2.1
    New-Item -ItemType directory -Force -Path ..\lib\
	Copy-Item -Force src\lua51.dll ..\lib\luajit-5.1.dll
	Copy-Item -Force src\lauxlib.h ..\include\luajit-2.1\
	Copy-Item -Force src\lua.h ..\include\luajit-2.1\
	Copy-Item -Force src\lua.hpp ..\include\luajit-2.1\
	Copy-Item -Force src\luaconf.h ..\include\luajit-2.1\
	Copy-Item -Force src\luajit.h ..\include\luajit-2.1\
	Copy-Item -Force src\lualib.h ..\include\luajit-2.1\
	Pop-Location
	Remove-Item -Recurse -Force LuaJIT	
}

function showHelp {
    "Usage: ./make-deps.sh [TARGET]"
    "Targets:"
	foreach ($i in $targets) {
		"    $i"
	}
}


switch ($argument) {
	"nuklear"          { updateNuklear         }
	"benchmark"        { updateBenchmark       }
	"fmt"              { updateFmt             }
	"xxhash"           { updateXxhash          }
	"skarupkeFHM"      { updateSkarupkeFHM     }
	"gtest"            { updateGTest           }
	"rapidxml"         { updateRapidXml        }
	"luajit"           { updateLuajit          }
	"--help" 	       { showHelp              }
	"all" {
		updateNuklear
		updateBenchmark
		updateFmt
		updateXxhash
		updateSkarupkeFHM
		updateGTest
		updateRapidXml
		updateLuajit
	}
	default {
		"Unknown target '$argument'"
		""
		showHelp
	}
}
