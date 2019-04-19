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
        RASSERTF(!isSectionExists(key), "Duplicate section [{}] in '{}' at line {}.", key, path, lineNum + 1);

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
using Path        = std::filesystem::path;
using PathRef     = Path&;
using PathCref    = const Path&;
using String      = ftl::String;
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


void deleteComments  (PathCref path, StrvVector& lines); // Delete comments and check all symbols
void processFileTask (PathCref path = cfg_detls::cfg_state().cfgPath);
void parseLinesTask  (PathCref path, StrvVector& lines);
void preprocessorTask(PathCref path, SizeT lineNum, StrViewCref line);
auto pairFromLine    (PathCref path, SizeT lineNum, StrViewCref line) -> StrViewPair;
auto unpackVariable  (PathCref path, SizeT lineNum, StrViewCref line) -> String;


auto unpackVariable (PathCref path, SizeT lineNum, StrViewCref line) -> String {
    auto ptr     = line.begin();
    auto pathStr = path.string();

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

                RASSERTF(ptr != line.cend(), "Empty key after '$' in file '{}' at line {}.", pathStr, lineNum + 1);

                RASSERTF(!is_digit(*ptr),
                         "Starting key with digit '{}' in file '{}' at line {}.",
                         *ptr, pathStr, lineNum + 1);

                auto start = ptr;

                while(!is_space(*ptr) && *ptr != ':' && ptr != line.cend()) {
                    RASSERTF(validate_name_symbol(*ptr),
                             "Invalid character '{}' in key after '$' in file '{}' at line {}.",
                             *ptr, pathStr, lineNum + 1);
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
                            "Empty key after ':' in file '{}' at line {}.", pathStr, lineNum + 1);

                    RASSERTF(!is_digit(*ptr),
                             "Starting key name with digit '{}' in file '{}' at line {}.",
                             *ptr, pathStr, lineNum + 1);

                    auto start2 = ptr;

                    while(!is_space(*ptr) && *ptr != ':' && ptr != line.cend()) {
                        RASSERTF(validate_name_symbol(*ptr),
                                 "Invalid character '{}' in key after '$' in file '{}' at line {}.",
                                 *ptr, pathStr, lineNum + 1);
                        ++ptr;
                    }

                    auto second = line.substr(start2 - line.cbegin(), ptr - start2);

                    RASSERTF(cfg_detls::cfgData().getSection(first).getParents().empty(),
                            "Attempt to dereference key '{}' from section [{}] with parent "
                            "in file '{}' at line {}.", second, first, pathStr, lineNum + 1);

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

auto pairFromLine(PathCref path, SizeT lineNum, StrViewCref line) -> StrViewPair {
    auto pathStr = path.string();
    auto ptr     = line.cbegin();

    if (skip_spaces_if_no_endl(ptr, line.cend()))
        return {};

    /////////// Read name

    auto start = ptr;

    RASSERTF(!is_digit(*ptr),
             "Starting key with digit '{}' in file '{}' at line {}.",
             *ptr, pathStr, lineNum + 1);

    while(!is_space(*ptr) && ptr != line.cend()) {
        RASSERTF(validate_name_symbol(*ptr),
                 "Invalid character '{}' in key definition in file '{}' at line {}.",
                 *ptr, pathStr, lineNum + 1);
        ++ptr;
    }

    auto key = line.substr(start - line.cbegin(), ptr - start);


    //////////// Read values (no space deleting inside values)

    RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
            "Missing value at key '{}' in file '{}' at line {}.", key, pathStr, lineNum + 1);

    RASSERTF(*ptr == '=',
            "Missing delimiter '=' at key '{}' in file '{}' at line {}.", key, pathStr, lineNum + 1);
        ++ptr;

    auto value = base::ConfigManager::
            remove_space_bounds_if_exists(line.substr(ptr - line.cbegin(), line.end() - ptr));

    RASSERTF(!value.empty(),
             "Missing value at key '{}' in file '{}' at line {}.", key, pathStr, lineNum + 1);

    return StrViewPair(key, value);
}


void deleteComments(PathCref path, StrvVector& lines) {
    auto strPath = path.string();

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
                        "Undefined char symbol '{}' [{}] in '{}' at line {}.",
                        *i, U32(U8(*i)), strPath, n + 1);

                if (*i == ';' || (*i == '/' && *(i + 1) == '/')) {
                    line = line.substr(0, i - line.begin());
                    break;
                }
            }
        }

        RASSERTF(!onSingleQuotes, "Missing second \' quote in '{}' at line {}.", strPath, n + 1);
        RASSERTF(!onDoubleQuotes, "Missing second \" quote in '{}' at line {}.", strPath, n + 1);
    }
}

void preprocessorTask(PathCref path, SizeT lineNum, StrViewCref line) {
    auto strPath = path.string();
    auto ptr     = line.cbegin();

    ++ptr; // skip '#'

    RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
            "Empty preprocessor directive in file '{}' at line {}.", strPath, lineNum + 1);

    auto start = ptr;

    while (!is_space(*ptr) && ptr != line.cend()) {
        RASSERTF(is_plain_text(*ptr),
                "Invalid character in preprocessor directive in file '{}' at line {}.",
                strPath, lineNum + 1);
        ++ptr;
    }

    auto first    = line.substr(start - line.cbegin(), ptr - start);
    auto backline = line.substr(ptr - line.cbegin(), line.cend() - ptr);

    if (first == "include") {
        auto appendPath =
                base::ConfigManager::remove_brackets_if_exists(
                        base::ConfigManager::remove_space_bounds_if_exists(backline));

        RASSERTF(!appendPath.empty(),
                "Empty path in include directive '{}' at line {}.", strPath, lineNum + 1);

        processFileTask(path.parent_path() / appendPath);
    } else {

        RASSERTF(0, "Unknown preprocessor directive '#{}' in '{}' at line {}.", first, strPath, lineNum + 1);
    }
}


void parseLinesTask(PathCref path, StrvVector& lines) {
    using cfg_detls::Section;
    using cfg_detls::cfgData;

    auto pathStr = path.string();

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

            RASSERTF(!is_digit(*ptr),
                     "Starting section with digit '{}' in file '{}' at line {}.",
                     *ptr, pathStr, n + 1);

            while(*ptr != ']' && ptr != line.cend()) {
                RASSERTF(validate_name_symbol(*ptr),
                        "Invalid character '{}' in section definition in file '{}' at line {}.",
                        *ptr, pathStr, n + 1);
                ++ptr;
            }

            RASSERTF(*ptr == ']',
                    "Missing close section bracket in '{}' at line {}.", pathStr, n + 1);

            currentSection =
                    &cfgData().addSection(pathStr, n, line.substr(start - line.cbegin(), ptr - start));

            ++ptr; // skip ']'

            if (skip_spaces_if_no_endl(ptr, line.cend()))
                continue;


            /////////////// Read parents

            RASSERTF(*ptr == ':',
                     "Unexpected symbol '{}' after section [{}] definition in '{}' at line {}.",
                     *ptr, currentSection->name(), pathStr, n+ 1);

            ++ptr; // skip ':'

            RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
                    "Missing parents sections after ':' in '{}' at line {}.",
                    pathStr, n + 1);



            while(ptr != line.end()) {
                auto start2 = ptr;

                RASSERTF(!is_digit(*ptr),
                         "Starting parent definition with digit '{}' in file '{}' at line {}.",
                         *ptr, pathStr, n + 1);

                while(!is_space(*ptr) && ptr != line.cend() && *ptr != ',') {
                    RASSERTF(validate_name_symbol(*ptr),
                             "Invalid character '{}' in parent definition in file '{}' at line {}.",
                             *ptr, pathStr, n + 1);
                    ++ptr;
                }

                currentSection->addParent(line.substr(start2 - line.cbegin(), ptr - start2));

                if (skip_spaces_if_no_endl(ptr, line.cend()))
                    break;

                RASSERTF(*ptr == ',',
                         "Missing ',' after parent definition in '{}' at line {}.", pathStr, n + 1);

                ++ptr;

                RASSERTF(!skip_spaces_if_no_endl(ptr, line.cend()),
                         "Missing parent parent definition after ',' in '{}' at line {}.",
                         pathStr, n + 1);
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
                         "Duplicate variable '{}' in global namespace in file '{}' at line {}.",
                         pair.first, pathStr, n + 1);

                cfgData().addValue(cfg_detls::GLOBAL_NAMESPACE, pair.first, var);
            }

            /*auto start = ptr;
            auto pair  = pairFromLine(path, n, line, '=');

            // Variables impl
            auto realVars = unpackVariables(path, n, pair.second);

            // Add pairs to sections
            if (currentSection) {
                currentSection->add(pair.first, realVars);
            } else {
                RASSERTF(!cfgData().section(base::cfg_detls::GLOBAL_NAMESPACE).isExists(pair.first),
                         "Duplicate variable '{}' in global namespace in file '{}' at line {}.",
                         pair.first, pathStr, n + 1);
                cfgData().addValue(base::cfg_detls::GLOBAL_NAMESPACE, pair.first, realVars);
            }*/
        }
    }
}

void processFileTask(PathCref path) {
    auto file  = base::readFileToString(path, true);       // no log mode
    auto lines = file.splitView({'\n', '\r', '\0'}, true); // do not delete empty strings

    deleteComments(path, lines);
    parseLinesTask(path, lines);
}

/*
auto unpackSingle    (PathCref path, SizeT lineNum, StrViewCref line) -> StrView;
auto unpackMulti
(PathCref path, SizeT lineNum, StrViewCref line, Char8 delim = ' ', SizeT required = 0) -> StrvVector;

auto unpackVariables (PathCref path, SizeT lineNum, StrViewCref str) -> ftl::String;
auto pairFromLine    (PathCref path, SizeT lineNum, StrViewCref line, Char8 delim = ' ') -> StrViewPair;
void deleteComments  (PathCref path, StrvVector& lines);
void preprocessorTask(PathCref path, SizeT lineNum, StrViewCref line);
void parseLinesTask  (PathCref path, StrvVector& lines);
void processFileTask (PathCref path = base::cfg_detls::cfg_state().cfgPath);


auto unpackSingle(PathCref path, SizeT lineNum, StrViewCref line) -> StrView {
    auto strPath = path.string();
    auto ptr     = line.cbegin();
    SKIP_WHITE_SPACE(ptr, line.cend());

    RASSERTF(ptr != line.cend(), "Missing value in '{}' at line {}.", strPath, lineNum + 1);

    auto start = ptr;
    auto end   = ptr;

    if (*ptr == '\'' || *ptr == '\"') {
        auto quote = *ptr++;
        start = ptr;

        while(ptr != line.cend() && *ptr != quote)
            ++ptr;

        RASSERTF(*ptr == quote, "Missing second {} quote in '{}' at line {}.", quote, strPath, lineNum + 1);

        end = ptr++;
    } else {
        SKIP_BEFORE_WHITE_SPACE(ptr, line.cend());
        end = ptr;
    }

    SKIP_WHITE_SPACE(ptr, line.cend());
    RASSERTF(ptr == line.cend(), "Redundant value in '{}' at line {}.", strPath, lineNum + 1);

    return line.substr(start - line.cbegin(), end - start);
}

auto unpackMulti(
        PathCref path,
        SizeT lineNum,
        StrViewCref line,
        Char8 delim,
        SizeT required) -> StrvVector
{
    auto vec     = StrvVector();
    auto strPath = path.string();
    auto ptr     = line.cbegin();
    SizeT valNum = 0;

    while (true) {
        SKIP_WHITE_SPACE(ptr, line.cend());

        if (ptr == line.cend())
            break;

        auto start = ptr;
        auto end   = ptr;

        if (*ptr == '\'' || *ptr == '\"') {
            auto quote = *ptr++;
            start = ptr;

            while (*ptr != quote && ptr != line.cend())
                ++ptr;

            RASSERTF(*ptr == quote, "Missing second {} quote in '{}' at line {}.", quote, strPath, lineNum + 1);

            end = ptr++;
        } else {
            while(!IS_WHITE_SPACE(*ptr) && *ptr != delim && ptr != line.cend())
                ++ptr;
            end = ptr;
        }

        vec.emplace_back(line.substr(start - line.cbegin(), end - start));

        SKIP_WHITE_SPACE(ptr, line.cend());
        if (delim != ' ' && ptr != line.cend()) {
            RASSERTF(*ptr == delim, "Missing delimiter '{}' after {} value in '{}' at line {}.",
                     delim, valNum + 1, strPath, lineNum + 1);
            ++ptr;
        }
        ++valNum;
    }

    RASSERTF(!vec.empty(), "Missing values in '{}' at line {}.", strPath, lineNum + 1);

    if (required != 0) {
        RASSERTF(valNum <= required, "Redundant value in '{}' at line {}. (provided {}, required {})",
                 strPath, lineNum + 1, valNum, required);
        RASSERTF(valNum >= required, "Missing value in '{}' at line {}. (provided {}, required {})",
                 strPath, lineNum + 1, valNum, required);
    }

    return std::move(vec);
}

void deleteComments(PathCref path, StrvVector& lines) {
    auto strPath = path.string();
    bool onComments = false;

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
                if (*i == ';' || (*i == '/' && *(i + 1) == '/')) {
                    line = line.substr(0, i - line.begin());
                    break;
                }
            }
        }

        RASSERTF(!onSingleQuotes, "Missing second \' quote in '{}' at line {}.", strPath, n + 1);
        RASSERTF(!onDoubleQuotes, "Missing second \" quote in '{}' at line {}.", strPath, n + 1);
    }
}

auto pairFromLine(PathCref path, SizeT lineNum, StrViewCref line, Char8 delim) -> StrViewPair {
    auto iter = line.cbegin();
    SKIP_WHITE_SPACE(iter, line.cend());
    if (iter == line.cend())
        return StrViewPair();

    auto start = iter;
    SKIP_BEFORE_WHITE_SPACE(iter, line.cend());
    if (iter == start)
        return StrViewPair();

    auto key = line.substr(start - line.cbegin(), iter - start);

    SKIP_WHITE_SPACE(iter, line.cend());
    if (iter == line.cend())
        return StrViewPair(key, "");

    if (delim != ' ' && delim != '\t') {
        RASSERTF(*iter == delim, "Missing delimiter '{}' in '{}' at line {}.", delim, path.string(), lineNum);
        ++iter;
        SKIP_WHITE_SPACE(iter, line.cend());
    }

    start = iter;
    auto back = line.end() - 1;

    bool onOneQuotes    = false;
    bool onDoubleQuotes = false;


    while(back != start && IS_WHITE_SPACE(*back))
        --back;

    if (back != start)
        return StrViewPair(key, line.substr(start - line.cbegin(), back - start + 1));
    else {
        RASSERTF(delim == ' ', "Missing value in '{}' at line {}.", path.string(), lineNum);
        return StrViewPair(key, "");
    }
}

void preprocessorTask(PathCref path, SizeT lineNum, StrViewCref line) {
    auto strPath = path.string();
    auto ptr = line.cbegin();

    ++ptr;
    SKIP_WHITE_SPACE(ptr, line.cend());

    auto start = ptr;
    SKIP_BEFORE_WHITE_SPACE(ptr, line.cend());

    RASSERTF(ptr != start, "Empty preprocessor directive in '{}' at line {}.", strPath, lineNum + 1);

    auto first    = line.substr(start - line.cbegin(), ptr - start);
    auto backline = line.substr(ptr - line.cbegin(), line.cend() - ptr);

    if (first == "include") {
        auto appendPath = unpackSingle(path, lineNum, backline);
        RASSERTF(!appendPath.empty(), "Empty path in include directive '{}' at line {}.", strPath, lineNum + 1);
        processFileTask(path.parent_path() / appendPath);
    } else {
        RASSERTF(0, "Unknown preprocessor directive '#{}' in '{}' at line {}.", first, strPath, lineNum + 1);
    }
}

// Unpack variables, delete brackets
auto unpackVariables(PathCref path, SizeT lineNum, StrViewCref str) -> ftl::String {
    auto res = ftl::String();
    res.reserve(str.length());

    auto ptr = str.cbegin();

    bool onSingleQuotes  = false;
    bool onDoubleQuotes  = false;
    bool firstSymbolFlag = true;
    bool onReadVar       = false;
    auto start           = ptr;

    for(; ptr != str.cend(); ++ptr) {
        if (*ptr == '\'' && !onDoubleQuotes) {
            onSingleQuotes  = !onSingleQuotes;
            firstSymbolFlag = true;
            continue;
        }
        if (*ptr == '\"' && !onSingleQuotes) {
            onDoubleQuotes = !onDoubleQuotes;
            firstSymbolFlag = true;
            continue;
        }

        if (!onSingleQuotes && !onDoubleQuotes) {
            if (*ptr == '$' && firstSymbolFlag) {
                onReadVar = true;
                start     = ptr + 1;
            }
            else if (*ptr == ' ' || *ptr == ',') {
                firstSymbolFlag = true;
                if (onReadVar && start != ptr) {
                    onReadVar = false;
                    res += base::cfg_detls::cfgData().getValue(
                            base::cfg_detls::GLOBAL_NAMESPACE, str.substr(start - str.cbegin(), ptr - start));
                }
                if (*ptr == ',')
                    res += ',';
                continue;
            }
            else if (!onReadVar)
                res += *ptr;
        } else
            res += *ptr;

        firstSymbolFlag = false;
    }

    if (onReadVar && start != ptr) {
        onReadVar = false;
        res += base::cfg_detls::cfgData().getValue(
                base::cfg_detls::GLOBAL_NAMESPACE, str.substr(start - str.cbegin(), ptr - start));
    }

    return std::move(res);
}

void parseLinesTask(PathCref path, StrvVector& lines) {
    using base::cfg_detls::Section;
    using base::cfg_detls::cfgData;

    auto pathStr = path.string();

    Section* currentSection = nullptr;

    for (SizeT n = 0; n < lines.size(); ++n) {
        auto& line = lines[n];
        auto ptr  = line.cbegin();

        SKIP_WHITE_SPACE(ptr, line.cend());

        if (ptr == line.cend())
            continue;

        // Read section
        if (*ptr == '[') {
            auto start = ++ptr;

            while(*ptr != ']' && ptr != line.cend())
                ++ptr;

            RASSERTF(*ptr == ']', "Missing close section bracket in '{}' at line {}.", pathStr, n + 1);

            currentSection = &cfgData().addSection(pathStr, n, line.substr(start - line.cbegin(), ptr - start));
            ++ptr;

            SKIP_WHITE_SPACE(ptr, line.cend());

            RASSERTF(*ptr == ':' || ptr == line.cend(),
                    "Unexpected symbol '{}' after section [{}] definition in '{}' at line {}.",
                    *ptr, currentSection->name(), pathStr, n+ 1);

            if (ptr == line.cend())
                continue;

            ++ptr;
            SKIP_WHITE_SPACE(ptr, line.cend());

            RASSERTF(ptr != line.end(), "Missing parents sections after ':' in '{}' at line {}.", pathStr, n + 1);

            while(ptr != line.end()) {
                auto start2 = ptr;
                while(!IS_WHITE_SPACE(*ptr) && ptr != line.cend() && *ptr != ',') { ++ptr; }

                currentSection->addParent(line.substr(start2 - line.cbegin(), ptr - start2));

                SKIP_WHITE_SPACE(ptr, line.cend());

                RASSERTF(*ptr == ',' || ptr == line.cend(),
                        "Missing ',' after parent definition in '{}' at line {}.", pathStr, n + 1);

                if (ptr == line.cend()) break;
                ++ptr;
                SKIP_WHITE_SPACE(ptr, line.cend());
            }
        } else if (*ptr == '#') {
            preprocessorTask(path, n, line.substr(ptr - line.begin()));
        } else {
            auto start = ptr;
            auto pair  = pairFromLine(path, n, line, '=');

            // Variables impl
            auto realVars = unpackVariables(path, n, pair.second);

            // Add pairs to sections
            if (currentSection) {
                currentSection->add(pair.first, realVars);
            } else {
                RASSERTF(!cfgData().section(base::cfg_detls::GLOBAL_NAMESPACE).isExists(pair.first),
                        "Duplicate variable '{}' in global namespace in file '{}' at line {}.",
                        pair.first, pathStr, n + 1);
                cfgData().addValue(base::cfg_detls::GLOBAL_NAMESPACE, pair.first, realVars);
            }
        }
    }
}

void processFileTask(PathCref path) {
    auto file  = base::readFileToString(path, true); // no log mode
    auto lines = file.splitView({'\n', '\r', '\0'}, true); // do not delete empty strings

    deleteComments  (path, lines);
    parseLinesTask  (path, lines);

}

 */

namespace base {
    auto splitView(const std::string_view& str, Char8 c) -> ftl::Vector<std::string_view> {
        auto vec   = ftl::Vector<std::string_view>();
        auto start = str.cbegin();

        bool onSingleQuotes = false;
        bool onDoubleQuotes = false;

        auto i = str.cbegin();
        for (; i != str.cend(); ++i) {
            if (*i == '\'' && !onDoubleQuotes)
                onSingleQuotes = !onSingleQuotes;
            else if (*i == '\"' && !onSingleQuotes)
                onDoubleQuotes = !onDoubleQuotes;
            else if (!onSingleQuotes && !onDoubleQuotes) {
                if (*i == ',') {
                    if ((*start == '\"' && *(i - 1) == '\"') ||
                        (*start == '\'' && *(i - 1) == '\''))
                        vec.emplace_back(str.substr(start - str.cbegin() + 1, i - start - 2));
                    else
                        vec.emplace_back(str.substr(start - str.cbegin(), i - start));
                    start = i + 1;
                }
            }
        }

        if (start != str.end()) {
            if ((*start == '\"' && *(i - 1) == '\"') ||
                (*start == '\'' && *(i - 1) == '\''))
                vec.emplace_back(str.substr(start - str.cbegin() + 1, i - start - 2));
            else
                vec.emplace_back(str.substr(start - str.cbegin() + 1, i - start));
        }

        return std::move(vec);
    }

    auto ConfigManager::readerUnpackMulti(
            StrViewCref name,
            StrViewCref section,
            StrViewCref str,
            SizeT       required) -> StrvVector
    {
        if
        auto vec = splitView(str, ',');

        // Remove brackets
        for (auto& s : vec) {
            if (s.size() > 1) {
                if (s[0] == '\"' && s.back() == '\"')
                    s = s.substr(1, s.size() - 2);
                else if (s[0] == '\'' && s.back() == '\'')
                    s = s.substr(1, s.size() - 2);
            }
        }
        RASSERTF(vec.size() == required || required == 0,
                "Wrong number of argumets in section [{}], value '{}'. Required: {}, provided: {}",
                section, name, required, vec.size());

        return std::move(vec);
    }


    void ConfigManager::load() {
        processFileTask();
        cfg_detls::cfgData().reloadParents();
    }
}