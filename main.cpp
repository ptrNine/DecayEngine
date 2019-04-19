#include <iostream>
#include "src/base/ftl/vector.hpp"
#include "src/base/ftl/string.hpp"
#include "src/base/configs.hpp"
#include "src/base/logs.hpp"

int main() {
    base::cfg::cfg();
    base::cfg_detls::cfgData()._print_info();
    //base::Log("ъеъ");
    //UIBuilder builder("/mnt/sda6-drive/Repos/DecayEngine/testGamedata/ui/gui.xml");
    //builder.present(nullptr);

    /*
    auto lua = lua::Context("/mnt/sda6-drive/Repos/DecayEngine/testGamedata/scripts");
    lua.doFile("main.lua");

    auto testFunc = lua::Function<double(double, double)>(lua, "hello");

    auto kek = testFunc(2.4, 3.6);
    std::cout << kek << std::endl;
     */



    return 0;
}