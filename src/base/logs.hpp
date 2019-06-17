#ifndef DECAYENGINE_LOGS_HPP
#define DECAYENGINE_LOGS_HPP

#include "defines.hpp"
#include "time.hpp"
#include "files.hpp"
#include "ftl/string.hpp"
#include <deque>

namespace base {
    namespace logs_dtls {
        class LoggerCreationState {
        public:
            bool onCreate  = false;
            bool isCreated = false;

            // Singleton impl
        public:
            LoggerCreationState(const LoggerCreationState&) = delete;
            LoggerCreationState(LoggerCreationState&&) = delete;
            LoggerCreationState& operator=(const LoggerCreationState&) = delete;

            static LoggerCreationState& instance() {
                static LoggerCreationState inst;
                return inst;
            }

        protected:
            LoggerCreationState () = default;
            ~LoggerCreationState() = default;
        };

        inline LoggerCreationState& log_state() {
            return LoggerCreationState::instance();
        }
    }

    class Logger {
    public:
        template <typename FmtT, typename... ArgsT>
        void operator()(FmtT format, ArgsT... args) {
            if (!_fw) return;

            auto msg  = ftl::String().sprintf(format, args...);
            auto time = timer().getSystemDateTime().to_string("[hh:mm:ss:xxx]: ");

            std::cout << msg << std::endl;

            _fw->write(time).write(msg).write("\n").flush();

            if (_isAttachedToJournal)
                addToJournal(time, msg);
        }

        auto& journalMaxSize      () const   { return _journalMaxSize; }
        auto& journalMaxSize      ()         { return _journalMaxSize; }
        auto& getJournal          ()         { return _journal; }
        auto& setAttachedToJournal(bool val) { _isAttachedToJournal = val; return *this; }


    protected:
        void addToJournal(const ftl::String& time, const ftl::String& str) {
            _journal.emplace_back(time, str);
            if (_journal.size() > _journalMaxSize)
                _journal.erase(_journal.begin());
        }

        SizeT _journalMaxSize     = 100;
        bool _isAttachedToJournal = false;
        std::deque<std::pair<ftl::String, ftl::String>> _journal;
        std::optional<FileWriter> _fw = {};

    // Singleton impl
    public:
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(const Logger&) = delete;

        static Logger& instance() {
            static Logger inst;
            return inst;
        }

    protected:
        Logger ();
        ~Logger();
    };

    template <typename FmtT, typename... ArgsT>
    inline void Log(FmtT format, ArgsT... args) {
        if (!logs_dtls::log_state().onCreate)
            Logger::instance()(format, args...);
    }

    inline void logger() {
        Logger::instance();
    }

#ifdef DE_DEBUG
    template <typename FmtT, typename... ArgsT>
    inline void DLog(FmtT format, ArgsT... args) {
        if (!logs_dtls::log_state().onCreate)
            Logger::instance()(format, args...);
    }
#else
    template <typename FmtT, typename... ArgsT>
    inline void DLog(FmtT format, ArgsT... args) {}
#endif
}



#endif //DECAYENGINE_LOGS_HPP
