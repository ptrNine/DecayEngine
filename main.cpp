

#include "src/base/ftl/string.hpp"

#include <iostream>
#include "src/luabind/LuaContext.hpp"
#include "src/luabind/Function.hpp"
#include "src/graphics/gui/UiBuilder.hpp"

#include <boost/hana.hpp>
#include <variant>


int main() {

    std::string_view a("dsfd");
    a.size();

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