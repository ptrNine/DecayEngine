#include "configs.hpp"
#include "assert.hpp"

namespace base::cfg_detls {
    auto Section::getValue(StringCref key) const -> StringCref {
        auto val = _pairs.find(key);
        RASSERTF(val != _pairs.end(), "Can't find key '{}' in section [{}]", key, _name);
        return val->second;
    }
    auto Section::getValue(StringCref key) -> StringRef {
        auto val = _pairs.find(key);
        RASSERTF(val != _pairs.end(), "Can't find key '{}' in section [{}]", key, _name);
        return val->second;
    }
    auto Section::value(StringCref key) -> StringRef {
        return _pairs[key];
    }
    auto Section::valueOpt(StringCref key) const -> std::optional<String> {
        auto val = _pairs.find(key);
        if (val == _pairs.end())
            return {};
        else
            return val->second;
    }
    void Section::add(StringCref key, StringCref value) {
        _pairs.emplace(key, value);
    }
    void Section::add(StringCref key, StringRval value) {
        _pairs.emplace(key, value);
    }
    void Section::addParent(StringCref parent) {
        _parents.push_back(parent);
    }
    bool Section::isExists(StringCref key) const {
        return _pairs.find(key) != _pairs.end();
    }
    void Section::reload() {
        for (const auto& sectStr : _parents) {
            auto& sect = cfgData().getSection(sectStr);

            if (!sect._alreadyReloaded && !sect.getParents().empty())
                sect.reload();

            auto pairs = sect.getMap();

            for (const auto& pair : pairs) {
                if (!isExists(pair.first))
                    add(pair.first, pair.second);
            }
        }

        _alreadyReloaded = true;
    }


    auto CfgData::getSection(StringCref key) const -> const Section& {
        auto sect = _sections.find(key);
        RASSERTF(sect != _sections.end(), "Can't find section [{}]", key);
        return sect->second;
    }
    auto CfgData::getSection(StringCref key) -> Section& {
        auto sect = _sections.find(key);
        RASSERTF(sect != _sections.end(), "Can't find section [{}]", key);
        return sect->second;
    }
    auto CfgData::section(StringCref key) -> Section& {
        auto& sect = _sections[key];

        if (sect.name().empty())
            sect.name() = key;

        return sect;
    }
    auto CfgData::sectionOpt(StringCref key) const -> std::optional<Section> {
        auto sect = _sections.find(key);
        if (sect == _sections.end())
            return {};
        else
            return sect->second;
    }
    bool CfgData::isSectionExists(StringCref key) const {
        return _sections.find(key) != _sections.end();
    }
    auto CfgData::addSection(StringCref path, SizeT lineNum, StringCref key) -> Section& {
        RASSERTF(!isSectionExists(key) ||
                 !key.compare(GLOBAL_NAMESPACE), "Duplicate section [{}] in {}:{}", key, path, lineNum + 1);

        auto& sect = _sections[key];
        sect.name() = key;

        return sect;
    }

    auto CfgData::getValue(StringCref sect, StringCref key) const -> StringCref {
        return getSection(sect).getValue(key);
    }
    auto CfgData::getValue(StringCref sect, StringCref key) -> StringRef {
        return getSection(sect).getValue(key);
    }
    auto CfgData::value(StringCref sect, StringCref key) -> StringRef {
        return section(sect).value(key);
    }
    auto CfgData::valueOpt(StringCref sect, StringCref key) const -> std::optional<String> {
        auto res1 = sectionOpt(sect);
        if (res1)
            return res1->valueOpt(key);
        return {};
    }
    bool CfgData::isValueExists(StringCref sect, StringCref key) const {
        auto res1 = isSectionExists(sect);
        if (res1)
            return getSection(sect).isExists(key);
        return false;
    }
    void CfgData::addValue(StringCref sect, StringCref key, StringCref value) {
        section(sect).add(key, value);
    }
    void CfgData::addValue(StringCref sect, StringCref key, StringRval value) {
        section(sect).add(key, value);
    }

    void CfgData::reloadParents() {
        for (auto& s : _sections)
            s.second.reload();
    }

    void CfgData::_print_info() const {
        std::cout << "Count: " << _sections.size() << std::endl;
        for (auto& p : _sections) {
            std::cout << "Section [" << p.first << "]";

            if (!p.second.getParents().empty()) {
                auto& parents = p.second.getParents();

                std::cout << ": ";
                for (SizeT i = 0; i < parents.size(); ++i) {
                    std::cout << parents[i];
                    if (i != parents.size() - 1)
                        std::cout << ", ";
                }
            }

            std::cout << std::endl;
            auto& map = p.second.getMap();
            for (auto& p2 : map) {
                std::cout << "\t" << p2.first << " = " << p2.second << std::endl;
            }
            std::cout << std::endl;
        }
    }
}


inline bool is_plain_text(Char8 c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

inline bool is_digit(Char8 c) {
    return c >= '0' && c <= '9';
}

inline bool is_space(Char8 c) {
    return c == ' ' || c == '\t';
}

inline bool is_bracket(Char8 c) {
    return c == '\'' || c == '\"';
}

inline bool is_legal_name_symbol(Char8 c) {
    return c == '@' || c == '.' || c =='/' || c == '\\';
}

inline bool is_symbol(Char8 c) {
    return c == ',' || c == ';' || c == '#' || c == '[' || c == ']' ||
           c == '-' || c == '+' || c == '=' || c == '$' || c == '{' ||
           c == '}' || c == ':' || is_legal_name_symbol(c) || is_bracket(c);
}

inline bool validate_name_symbol(Char8 c) {
    return is_plain_text(c) || is_digit(c) || is_legal_name_symbol(c);
}

inline bool validate_keyval(Char8 c) {
    return validate_name_symbol(c) || is_space(c) || c == '-' || c == '+';
}

inline bool validate_symbol(Char8 c) {
    return is_space(c) || is_digit(c) || is_plain_text(c) || is_symbol(c);
}


// Skip spaces, return true if end passed
template <typename IterT>
inline bool skip_spaces_if_no_endl(IterT& ptr, IterT end) {
    while(is_space(*ptr) && ptr != end)
        ++ptr;

    return ptr == end;
}

// Skip spaces, return true if beg passed
template <typename IterT>
inline bool rskip_spaces_if_no_beg(IterT& ptr, IterT beg) {
    while(is_space(*ptr) && ptr != beg)
        --ptr;

    return ptr == beg;
}

#define IS_WHITE_SPACE(CH)                 ((CH) == ' ' || (CH) == '\t')
#define SKIP_WHITE_SPACE(ITR, ENDL)        while(IS_WHITE_SPACE(*(ITR)) && (ITR) != (ENDL)) ++(ITR)
#define SKIP_BEFORE_WHITE_SPACE(ITR, ENDL) while(!IS_WHITE_SPACE(*(ITR)) && (ITR) != (ENDL)) ++(ITR)


using StrvStrvMap = ska::flat_hash_map<std::string_view, std::string_view>;
using StrvVector  = ftl::Vector<std::string_view>;
using StrView     = std::string_view;
using StrViewCref = const std::string_view&;
using StrViewPair = std::pair<StrView, StrView>;
using String      = ftl::String;
using StrRef      = String&;
using StrCref     = const String&;
namespace cfg_detls = base::cfg_detls;


auto base::ConfigManager::remove_space_bounds_if_exists(StrViewCref str) -> StrView {
    auto start = str.cbegin();
    auto last  = str.cend() - 1;

    if (!skip_spaces_if_no_endl(start, last)) {
        if (!rskip_spaces_if_no_beg(last, start))
            return str.substr(start - str.cbegin(), last - start + 1);
    }

    return {};
}

auto base::ConfigManager::remove_brackets_if_exists(StrViewCref str) -> StrView {
    if (str.length() > 1 && is_bracket(str.front()) && str.back() == str.front())
        return str.substr(1, str.length() - 2);
    return str;
}


void deleteComments  (StrCref path, StrvVector& lines); // Delete comments and check all symbols
void processFileTask (StrCref path);
void processFileTask ();
void parseLinesTask  (StrCref path, StrvVector& lines);
void preprocessorTask(StrCref path, SizeT lineNum, StrViewCref line);
auto pairFromLine    (StrCref path, SizeT lineNum, StrViewCref line) -> StrViewPair;
auto unpackVariable  (StrCref path, SizeT lineNum, StrViewCref line) -> String;


auto unpackVariable (StrCref path, SizeT lineNum, StrViewCref line) -> String {
    auto ptr = line.begin();

    bool onSingleQuotes = false;
    bool onDoubleQuotes = false;

    auto result = String();
    result.reserve(line.length());

    for (; ptr != line.cend();) {
        if (*ptr == '\'' && !onDoubleQuotes)
            onSingleQuotes = !onSingleQuotes;

        else if (*ptr == '\"' && !onSingleQuotes)
            onDoubleQuotes = !onDoubleQuotes;

        else if (onSingleQuotes || onDoubleQuotes) {
            result.push_back(*ptr);
        }
        else {
            if (*ptr == '$') {
                ++ptr; // Skip '$'

                ////////// Read key

                RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
                         "Empty key after '$' in {}:{}",
                         path, lineNum + 1);

                RASSERTF(!is_digit(*ptr) && !is_legal_name_symbol(*ptr),
                         "Starting key with symbol '{}' in {}:{}",
                         *ptr, path, lineNum + 1);

                auto start = ptr;

                while(!is_space(*ptr) && *ptr != ':' && ptr != line.cend() && *ptr != '}' && *ptr != ',') {
                    RASSERTF(validate_name_symbol(*ptr),
                             "Invalid character '{}' in key after '$' in {}:{}",
                             *ptr, path, lineNum + 1);
                    ++ptr;
                }

                auto val   = StrView();
                auto first = line.substr(start - line.cbegin(), ptr - start);

                skip_spaces_if_no_endl(ptr, line.cend());

                ////////// Dereference key
                if (ptr == line.cend() || *ptr != ':')
                    //////// Read value from global namespaces
                    val = cfg_detls::cfgData().getValue(cfg_detls::GLOBAL_NAMESPACE, first);
                else if (*ptr == ':'){
                    //////// Read value from section (only no-parents section supported)
                    ++ptr; // skip ':'

                    RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
                            "Empty key after ':' in {}:{}", path, lineNum + 1);

                    RASSERTF(!is_digit(*ptr) && !is_legal_name_symbol(*ptr),
                             "Starting key name with symbol '{}' in {}:{}",
                             *ptr, path, lineNum + 1);

                    auto start2 = ptr;

                    while(!is_space(*ptr) && *ptr != ':' && ptr != line.cend()) {
                        RASSERTF(validate_name_symbol(*ptr),
                                 "Invalid character '{}' in key after '$' in {}:{}",
                                 *ptr, path, lineNum + 1);
                        ++ptr;
                    }

                    auto second = line.substr(start2 - line.cbegin(), ptr - start2);

                    RASSERTF(cfg_detls::cfgData().getSection(first).getParents().empty(),
                            "Attempt to dereference key '{}' from section [{}] with parent "
                            "in {}:{}", second, first, path, lineNum + 1);

                    val = cfg_detls::cfgData().getValue(first, second);
                }

                result += base::ConfigManager::remove_brackets_if_exists(val);
                continue;
            }
            else if (!is_space(*ptr)) {
                result.push_back(*ptr);
            }
        }

        ++ptr;
    }

    return std::move(result);
}

auto pairFromLine(StrCref path, SizeT lineNum, StrViewCref line) -> StrViewPair {
    auto ptr = line.cbegin();

    if (skip_spaces_if_no_endl(ptr, line.cend()))
        return {};

    /////////// Read name

    auto start = ptr;

    RASSERTF(!is_digit(*ptr) && !is_legal_name_symbol(*ptr),
             "Starting key with symbol '{}' in {}:{}",
             *ptr, path, lineNum + 1);

    while(!is_space(*ptr) && *ptr != '=' && ptr != line.cend()) {
        RASSERTF(validate_name_symbol(*ptr),
                 "Invalid character '{}' in key definition in {}:{}.",
                 *ptr, path, lineNum + 1);
        ++ptr;
    }

    auto key = line.substr(start - line.cbegin(), ptr - start);


    //////////// Read values (no space deleting inside values)

    RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
            "Missing value at key '{}' in {}:{}", key, path, lineNum + 1);

    RASSERTF(*ptr == '=',
            "Missing delimiter '=' at key '{}' in {}:{}", key, path, lineNum + 1);
        ++ptr;

    auto value = base::ConfigManager::
            remove_space_bounds_if_exists(line.substr(ptr - line.cbegin(), line.end() - ptr));

    RASSERTF(!value.empty(),
             "Missing value at key '{}' in {}:{}", key, path, lineNum + 1);

    return StrViewPair(key, value);
}


void deleteComments(StrCref path, StrvVector& lines) {
    for (SizeT n = 0; n < lines.size(); ++n) {
        bool onSingleQuotes = false;
        bool onDoubleQuotes = false;

        auto& line = lines[n];
        auto  i    = line.begin();

        for (; i != line.end(); ++i) {
            if (*i == '\'' && !onDoubleQuotes)
                onSingleQuotes = !onSingleQuotes;

            else if (*i == '\"' && !onSingleQuotes)
                onDoubleQuotes = !onDoubleQuotes;

            else if (!onSingleQuotes && !onDoubleQuotes) {
                RASSERTF(validate_symbol(*i),
                        "Undefined char symbol '{}' [{}] in {}:{}",
                        *i, U32(U8(*i)), path, n + 1);

                if (*i == ';' || (*i == '/' && *(i + 1) == '/')) {
                    line = line.substr(0, i - line.begin());
                    break;
                }
            }
        }

        RASSERTF(!onSingleQuotes, "Missing second \' quote in {}:{}", path, n + 1);
        RASSERTF(!onDoubleQuotes, "Missing second \" quote in {}:{}", path, n + 1);
    }
}

void preprocessorTask(StrCref path, SizeT lineNum, StrViewCref line) {
    auto ptr = line.cbegin();

    ++ptr; // skip '#'

    RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
            "Empty preprocessor directive in {}:{}", path, lineNum + 1);

    auto start = ptr;

    while (!is_space(*ptr) && ptr != line.cend()) {
        RASSERTF(is_plain_text(*ptr),
                "Invalid character in preprocessor directive in {}:{}",
                 path, lineNum + 1);
        ++ptr;
    }

    auto first    = line.substr(start - line.cbegin(), ptr - start);
    auto backline = line.substr(ptr - line.cbegin(), line.cend() - ptr);

    if (first == "include") {
        auto appendPath =
                base::ConfigManager::remove_brackets_if_exists(
                        base::ConfigManager::remove_space_bounds_if_exists(backline));

        RASSERTF(!appendPath.empty(),
                "Empty path in include directive in {}:{}", path, lineNum + 1);

        processFileTask(path.parent_path() / appendPath);
    } else {

        RASSERTF(0, "Unknown preprocessor directive '#{}' in {}:{}", first, path, lineNum + 1);
    }
}


void parseLinesTask(StrCref path, StrvVector& lines) {
    using cfg_detls::Section;
    using cfg_detls::cfgData;

    Section* currentSection = nullptr;

    for (SizeT n = 0; n < lines.size(); ++n) {
        auto& line = lines[n];
        auto  ptr  = line.cbegin();

        if (skip_spaces_if_no_endl(ptr, line.cend()))
            continue;
        ////////////////////////////////////// Section /////////////////////////////////////////
        if (*ptr == '[') {
            auto start = ++ptr;

            ////////////// Read section

            RASSERTF(!is_digit(*ptr) && !is_legal_name_symbol(*ptr),
                     "Starting section with symbol '{}' in {}:{}",
                     *ptr, path, n + 1);

            while(*ptr != ']' && ptr != line.cend()) {
                RASSERTF(validate_name_symbol(*ptr),
                        "Invalid character '{}' in section definition in {}:{}",
                        *ptr, path, n + 1);
                ++ptr;
            }

            RASSERTF(*ptr == ']',
                    "Missing close section bracket in '{}' at line {}.", path, n + 1);

            currentSection =
                    &cfgData().addSection(path, n, line.substr(start - line.cbegin(), ptr - start));

            ++ptr; // skip ']'

            if (skip_spaces_if_no_endl(ptr, line.cend()))
                continue;


            /////////////// Read parents

            RASSERTF(*ptr == ':',
                     "Unexpected symbol '{}' after section [{}] definition in '{}' at line {}.",
                     *ptr, currentSection->name(), path, n+ 1);

            ++ptr; // skip ':'

            RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
                    "Missing parents sections after ':' in '{}' at line {}.",
                    path, n + 1);



            while(ptr != line.end()) {
                auto start2 = ptr;

                RASSERTF(!is_digit(*ptr) && !is_legal_name_symbol(*ptr),
                         "Starting parent definition with symbol '{}' in {}:{}",
                         *ptr, path, n + 1);

                while(!is_space(*ptr) && ptr != line.cend() && *ptr != ',') {
                    RASSERTF(validate_name_symbol(*ptr),
                             "Invalid character '{}' in parent definition in {}:{}",
                             *ptr, path, n + 1);
                    ++ptr;
                }

                currentSection->addParent(line.substr(start2 - line.cbegin(), ptr - start2));

                if (skip_spaces_if_no_endl(ptr, line.cend()))
                    break;

                RASSERTF(*ptr == ',',
                         "Missing ',' after parent definition in '{}' at line {}.", path, n + 1);

                ++ptr;

                RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
                         "Missing parent parent definition after ',' in '{}' at line {}.",
                         path, n + 1);
            }
        }

        //////////////////////////// Preprocessor task //////////////////////////////////////
        else if (*ptr == '#') {
            preprocessorTask(path, n, line.substr(ptr - line.begin()));
        }

        //////////////////////////////// Read variables /////////////////////////////////////
        else {
            auto pair = pairFromLine(path, n, line);

            auto var  = unpackVariable(path, n, pair.second);

            if (currentSection)
                currentSection->add(pair.first, var);
            else {
                RASSERTF(!cfgData().section(cfg_detls::GLOBAL_NAMESPACE).isExists(pair.first),
                         "Duplicate variable '{}' in global namespace in {}:{}",
                         pair.first, path, n + 1);

                cfgData().addValue(cfg_detls::GLOBAL_NAMESPACE, pair.first, var);
            }
        }
    }
}

void processFileTask(StrCref path) {
    auto file  = base::readFileToString(path);       // no log mode
    auto lines = file.splitView({'\n', '\r', '\0'}, true); // do not delete empty strings

    deleteComments(path, lines);
    parseLinesTask(path, lines);
}

void processFileTask() {
    for (auto& path : cfg_detls::cfg_state().getEntries())
        processFileTask(path);
}

namespace base {
    auto ConfigManager::readerUnpackMulti(
            StrViewCref name,
            StrViewCref section,
            StrViewCref str,
            SizeT       required) -> StrvVector
    {
        StrvVector vec;

        auto ptr   = str.cbegin();

        while (ptr != str.cend()) {
            if (*ptr == '{') {
                S32 entryLevel = 0;
                auto start = ptr;

                bool onSingleQuotes = false;
                bool onDoubleQuotes = false;

                for (; ptr != str.cend(); ++ptr) {
                    if (*ptr == '\'' && !onDoubleQuotes)
                        onSingleQuotes = !onSingleQuotes;

                    else if (*ptr == '\"' && !onSingleQuotes)
                        onDoubleQuotes = !onDoubleQuotes;

                    else if (!onSingleQuotes && !onDoubleQuotes) {
                        if (*ptr == '{') {
                            ++entryLevel;
                        } else if (*ptr == '}') {
                            --entryLevel;

                            if (entryLevel == 0) {
                                if (ptr + 1 != str.cend()) {
                                    RASSERTF(*(ptr + 1) != '}',
                                             "Redundant '{}' at key '{}' in section [{}].",
                                             '}', name, section);

                                    RASSERTF(*(ptr + 1) == ',',
                                             "Missing ',' at key '{}' in section [{}].",
                                             name, section);
                                }

                                break;
                            }
                        }
                    }
                }

                RASSERTF(entryLevel == 0,
                         "Missing close '{}' at key '{}' in section [{}].",
                         '}', name, section);

                if (*ptr == '}' && ptr != str.cend())
                    ++ptr;


                vec.emplace_back(str.substr(start - str.cbegin(), ptr - start));

                if (ptr != str.cend())
                    ++ptr;

            }
            else {
                auto start = ptr;

                bool onSingleQuotes = false;
                bool onDoubleQuotes = false;

                for (; ptr != str.cend(); ++ptr) {
                    if (*ptr == '\'' && !onDoubleQuotes)
                        onSingleQuotes = !onSingleQuotes;

                    else if (*ptr == '\"' && !onSingleQuotes)
                        onDoubleQuotes = !onDoubleQuotes;

                    else if (!onSingleQuotes && !onDoubleQuotes) {
                        if (*ptr == ',')
                            break;

                        RASSERTF(validate_keyval(*ptr),
                                 "Undefined char symbol '{}' at key '{}' in section [{}].",
                                 *ptr, name, section);
                    }
                }

                vec.emplace_back(str.substr(start - str.cbegin(), ptr - start));

                if (ptr != str.cend())
                    ++ptr;
            }
        }

        if (vec.size() == 1 && vec[0].size() > 1 && vec[0].front() == '{' && vec[0].back() == '}')
            return readerUnpackMulti(name, section, vec[0].substr(1, vec[0].size() - 2), required);

        RASSERTF(required == 0 || required == vec.size(),
                 "Wrong number of values at key '{}' in section [{}]. Provided {}, required {}.",
                 name, section, vec.size(), required);

        return std::move(vec);
    }


    void ConfigManager::load() {
        processFileTask();
        cfg_detls::cfgData().reloadParents();
    }
}