
#include "src/base/ftl/string.hpp"
//#include "src/base/configs.hpp"
//#include <flat_hash_map.hpp>
#include "3rd/include/flat_hash_map.hpp"
#include "src/base/time.hpp"
#include "src/base/ftl/array.hpp"

#include <fmt/formatTest.h>
#include <iostream>

int main() {
    ftl::String str;

    auto date = base::timer().getSystemDateTime();

    str =  date.to_string("Сегодня DD.M.YYYY. Время hh:mm:ss и xxx миллисекунд YYYY");

    std::cout << str << std::endl;


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