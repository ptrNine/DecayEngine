#include "filesystem.hpp"

#include "assert.hpp"

#include <string>
#include <array>
#include <iostream>
#include <string_view>

using Char8 = char;

auto getExeLocation   () -> ftl::String;
int  recursiveMakeDir (const std::string_view& path);



auto base::fs::current_path() -> ftl::String {
    return getExeLocation();
}

void base::fs::create_dir(const std::string_view& path) {
    recursiveMakeDir(path);
}

auto base::fs::to_data_path(const std::string_view& path) -> ftl::String {
    return current_path().parent_path() / path;
}



//////////////////////////// UNIX
#ifdef __unix__

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define DE_PATH_MAX 8192


int makeDirAbort(const std::string_view& path) {
    RABORTF("Can't create directory '{}'. {}", path, strerror(errno));
    return -1;
}

int recursiveMakeDir(const std::string_view& path) {
    if (path.empty() || path == "." || path == "/")
        return 0;


    auto findPos = path.rfind('/');
    if (findPos == path.npos)
        return 0;

    auto parent = path.substr(0, findPos);

    if (recursiveMakeDir(parent) == -1 && errno != EEXIST)
        return -1;

    return (mkdir(std::string(path).c_str(), 0777) == -1 && errno != EEXIST) ?
           makeDirAbort(path) : 0;
}


auto getExeLocation() -> ftl::String {
    auto result = std::array<Char8, DE_PATH_MAX>();
    auto count  = readlink( "/proc/self/exe", result.data(), DE_PATH_MAX);

    if (count < 0)
        count = 0;

    // Drop exe name
    for (auto p = result.crbegin() + DE_PATH_MAX - count; p != result.crend() && *p != '/'; ++p)
        --count;

    return ftl::String(result.data(), static_cast<SizeT>(count));
}

#undef DE_PATH_MAX

///////////////////////////// WINDOWS
#elif _WIN32

#include <codecvt>
#include <locale>
#include <algorithm>
#include <windows.h>
#define DE_PATH_MAX 8192

ftl::String GetLastErrorAsString(int rc) {
    if(rc == 0)
        return {};

    LPSTR messageBuffer = nullptr;
    SizeT size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, rc, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    auto message =  ftl::String(messageBuffer, size);

    LocalFree(messageBuffer);

    return message;
}

int makeDirAbort(const std::string_view& path, int rc) {
    RABORTF("Can't create directory '{}'. {}", path, GetLastErrorAsString(rc));
    return -1;
}

int recursiveMakeDir(const std::string_view& path) {
    if (path.empty() || path == "." || path == "/")
            return 0;

    auto findPos = path.rfind('/');
    if (findPos == path.npos)
        return 0;

    auto parent = path.substr(0, findPos);

    if (recursiveMakeDir(parent) == -1 && GetLastError() != ERROR_ALREADY_EXISTS)
        return -1;

	CreateDirectory(std::string(path).c_str(), NULL);
	auto rc = GetLastError();

    return (rc != ERROR_ALREADY_EXISTS && rc != 0) ?
    makeDirAbort(path, rc) : 0;

}

auto getExeLocation() -> ftl::String {
    auto result = std::array<wchar_t, DE_PATH_MAX>();
    auto count  = GetModuleFileNameW(NULL, result.data(), DE_PATH_MAX);
    result[count] = L'\0';

    if (count < 0)
        count = 0;

    // Drop exe name
    for (auto p = result.crbegin() + DE_PATH_MAX - count; p != result.crend() && *p != L'\\'; ++p)
        --count;

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;

    auto res = convert.to_bytes(result.data(), result.data() + count);
    std::replace(res.begin(), res.end(), '\\', '/');

    return res;
}
#undef DE_PATH_MAX


//////////////////////////////// FUCK YOU
#else
#error "Your system doesn't supported!"
#endif