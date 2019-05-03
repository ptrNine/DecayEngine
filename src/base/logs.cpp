#include "logs.hpp"

#include "filesystem.hpp"

#include "configs.hpp"

base::Logger::Logger()  {
    logs_dtls::log_state().onCreate = true;

    // Todo: read log path from cfg
    auto path = ftl::String(base::fs::current_path().parent_path()) /
            cfg::force_read_ie<ftl::String>("logs_dir", "force_log");
    auto name = ftl::String("log_");
    name += timer().getSystemDateTime().to_string("DD_MM_YYYY__hh:mm:ss.log");
    _fw.emplace(FileWriter(path / name));

    _fw->write("!!! LOG START   ")
        .write(timer().getSystemDateTime().to_string("DD.MM.YYYY hh:mm:ss:xxx.\n\n"))
        .flush();

    logs_dtls::log_state().onCreate = false;
    logs_dtls::log_state().isCreated = true;
}

base::Logger::~Logger() {
    _fw->write("\n!!! LOG DESTROY ")
        .write(timer().getSystemDateTime().to_string("DD.MM.YYYY hh:mm:ss:xxx.\n"))
        .flush();
}