#include <gtest/gtest.h>
#include "../base/configs.hpp"

TEST(ConfigTests, TestSection) {
    namespace cfg = base::cfg;
    using String  = ftl::String;

    cfg::force_set_cfg_path(
            std::filesystem::current_path().parent_path().parent_path().parent_path() / "fs.cfg");

    ASSERT_EQ(cfg::read<String>("one", "test_section"), " global val2226 global val");
}

