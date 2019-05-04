#pragma once

#include <flat_hash_map.hpp>
#include "filesystem.hpp"
#include "defines.hpp"
#include "ftl/string.hpp"
#include "ftl/vector.hpp"
#include "aton.hpp"
#include "ftl/vector2.hpp"
#include "ftl/vector3.hpp"

#define IA inline auto
#define SIA static inline auto

namespace base {

    class ConfigManager;

    namespace cfg_detls {
        static constexpr inline std::string_view GLOBAL_NAMESPACE = "__global";

        static inline auto DEFAULT_CFG_PATH() {
            return fs::current_path().parent_path() / "fs.cfg";
        }

        class CfgCreationState {
            friend ConfigManager;
        public:
            void clearCfgEntries() {
                _cfgEntries.clear();
            }

            void addCfgEntry(const ftl::String& path) {
                _cfgEntries.push_back(path);
            }

            bool onCreate () const { return _onCreate; }
            bool isCreated() const { return _isCreated; }

            const auto& getEntries() const {
                return _cfgEntries;
            }

        private:
            bool _onCreate  = false;
            bool _isCreated = false;

            ftl::Vector<ftl::String> _cfgEntries;

            // Singleton impl
        public:
            CfgCreationState(const CfgCreationState&) = delete;
            CfgCreationState(CfgCreationState&&) = delete;
            CfgCreationState& operator=(const CfgCreationState&) = delete;

            static CfgCreationState& instance() {
                static CfgCreationState inst;
                return inst;
            }

        private:
            CfgCreationState () {
                // Add default cfgPath
                _cfgEntries.emplace_back(DEFAULT_CFG_PATH());
            }
            ~CfgCreationState() = default;
        };


        inline CfgCreationState& cfg_state() {
            return CfgCreationState::instance();
        }

        using String        = ftl::String;
        using StringCref    = const String&;
        using StringRef     = String&;
        using StringRval    = String&&;
        using StrStrMap     = ska::flat_hash_map<String, String>;
        using StrSectionMap = ska::flat_hash_map<String, class Section>;
        using StrVector     = ftl::Vector<String>;

        class Section {
        public:
            auto getValue  (StringCref key) const -> StringCref;      // Get value (assert if not exists)
            auto getValue  (StringCref key) -> StringRef;             // Get value (assert if not exists)
            auto value     (StringCref key) -> StringRef;             // Get or create value
            auto valueOpt  (StringCref key) const -> std::optional<String>; // Get value or nullopt
            bool isExists  (StringCref key) const;
            void add       (StringCref key, StringCref value);
            void add       (StringCref key, StringRval value);
            void addParent (StringCref parent);
            auto getMap    () -> StrStrMap&             { return _pairs; }
            auto getMap    () const -> const StrStrMap& { return _pairs; }
            auto getParents() -> StrVector&             { return _parents; }
            auto getParents() const -> const StrVector& { return _parents; }

            auto& name()       { return _name; }
            auto& name() const { return _name; }

            void reload();

        private:
            StrStrMap _pairs;
            StrVector _parents;
            String    _name;
            String    _file;
            bool      _alreadyReloaded = false;
        };

        class CfgData {
        public:
            auto getSection      (StringCref key) const  -> const Section&;
            auto getSection      (StringCref key) -> Section&;
            auto section         (StringCref key) -> Section&;
            auto sectionOpt      (StringCref key) const -> std::optional<Section>;
            bool isSectionExists (StringCref key) const;
            auto addSection      (StringCref path, SizeT lineNum, StringCref key) -> Section&;

            auto getValue      (StringCref section, StringCref key) const -> StringCref;
            auto getValue      (StringCref section, StringCref key) -> StringRef;
            auto value         (StringCref section, StringCref key) -> StringRef;
            auto valueOpt      (StringCref section, StringCref key) const -> std::optional<String>;
            bool isValueExists (StringCref section, StringCref key) const;
            void addValue      (StringCref section, StringCref key, StringCref value);
            void addValue      (StringCref section, StringCref key, StringRval value);

            void reloadParents ();

            void _print_info() const;

        private:
            StrSectionMap _sections;

            // Singleton impl
        public:
            CfgData(const CfgData&) = delete;
            CfgData& operator= (const CfgData&) = delete;

            static CfgData& instance() {
                static CfgData inst;
                return inst;
            }

        private:
            CfgData()  = default;
            ~CfgData() = default;
        };

        inline CfgData& cfgData() { return CfgData::instance(); }
    } // namespace cfg_detls


    class ConfigManager {
    public:
        using StrvStrvMap = ska::flat_hash_map<std::string_view, std::string_view>;
        using StrvVector  = ftl::Vector<std::string_view>;
        using StrView     = std::string_view;
        using StrViewCref = const std::string_view&;
        using StrViewPair = std::pair<StrView, StrView>;

    public:
        template <typename... Ts>
        IA read(StrViewCref name, StrViewCref section = cfg_detls::GLOBAL_NAMESPACE) {
            return force_read<Ts...>(name, section);
        }

        // Read if section or value exist, else return default value
        template <typename T>
        IA read_ie(StrViewCref name, const T& def_val) {
            return force_read_ie<T>(name, def_val);
        }
        template <typename T1, typename T2, typename... Ts>
        IA read_ie(StrViewCref name, const std::tuple<T1, T2, Ts...>& def_vals) {
            return force_read_ie<T1, T2, Ts...>(name, def_vals);
        }
        template <typename T>
        IA read_ie(StrViewCref name, StrViewCref section, const T& def_val) {
            return force_read_ie<T>(name, section, def_val);
        }
        template <typename T1, typename T2, typename... Ts>
        IA read_ie(StrViewCref name, StrViewCref section, const std::tuple<T1, T2, Ts...>& def_vals) {
            return force_read_ie<T1, T2, Ts...>(name, section, def_vals);
        }

        // Read if value exist. Return default value if not, assert if section doesn't exists
        template <typename T>
        IA read_ive(StrViewCref name, const T& def_val) {
            return force_read_ive<T>(name, def_val);
        }
        template <typename T1, typename T2, typename... Ts>
        IA read_ive(StrViewCref name, const std::tuple<T1, T2, Ts...>& def_vals) {
            return force_read_ive<T1, T2, Ts...>(name, def_vals);
        }
        template <typename T>
        IA read_ive(StrViewCref name, StrViewCref section, const T& def_val) {
            return force_read_ive<T>(name, section, def_val);
        }
        template <typename T1, typename T2, typename... Ts>
        IA read_ive(StrViewCref name, StrViewCref section, const std::tuple<T1, T2, Ts...>& def_vals) {
            return force_read_ive<T1, T2, Ts...>(name, section, def_vals);
        }


        // Static methods

        // Read if section or value exist, else return default value
        template <typename T>
        static IA force_read_ie(StrViewCref name, const T& def_val) {
            return force_read_ie<T>(name, cfg_detls::GLOBAL_NAMESPACE, def_val);
        }
        template <typename T1, typename T2, typename... Ts>
        static IA force_read_ie(StrViewCref name, const std::tuple<T1, T2, Ts...>& def_vals) {
            return force_read_ie<T1, T2, Ts...>(name, cfg_detls::GLOBAL_NAMESPACE, def_vals);
        }
        template <typename T>
        static T force_read_ie(StrViewCref name, StrViewCref section, const T& def_val) {
            auto str = cfg_detls::cfgData().valueOpt(section, name);
            if (str)
                return superCast<T>(*str, name, section);
            else
                return def_val;
        }
        template <typename T1, typename T2, typename... Ts>
        static auto force_read_ie(StrViewCref name, StrViewCref section, const std::tuple<T1, T2, Ts...>& def_vals) {
            auto str = cfg_detls::cfgData().valueOpt(section, name);
            if (str)
                return superCast<T1, T2, Ts...>(*str, name, section);
            else
                return def_vals;
        }

        // Read if value exist. Return default value if not, assert if section doesn't exists
        template <typename T>
        static IA force_read_ive(StrViewCref name, const T& def_val) {
            return force_read_ive<T>(name, cfg_detls::GLOBAL_NAMESPACE, def_val);
        }
        template <typename T1, typename T2, typename... Ts>
        static IA force_read_ive(StrViewCref name, const std::tuple<T1, T2, Ts...>& def_vals) {
            return force_read_ive<T1, T2, Ts...>(name, cfg_detls::GLOBAL_NAMESPACE, def_vals);
        }
        template <typename T>
        static T force_read_ive(StrViewCref name, StrViewCref section, const T& def_val) {
            auto sect = cfg_detls::cfgData().getSection(section);
            auto str  = sect.valueOpt(name);
            if (str)
                return superCast<T>(*str, name, section);
            else
                return def_val;
        }
        template <typename T1, typename T2, typename... Ts>
        static auto force_read_ive(StrViewCref name, StrViewCref section, const std::tuple<T1, T2, Ts...>& def_vals) {
            auto sect = cfg_detls::cfgData().getSection(section);
            auto str  = sect.valueOpt(name);
            if (str)
                return superCast<T1, T2, Ts...>(*str, name, section);
            else
                return def_vals;
        }



        template <typename T>
        static T force_read(StrViewCref name, StrViewCref section = cfg_detls::GLOBAL_NAMESPACE) {
            return superCast<T>(readString(name, section), name, section);
        }

        template <typename T1, typename T2, typename... Ts>
        static auto force_read(StrViewCref name, StrViewCref section = cfg_detls::GLOBAL_NAMESPACE) {
            auto str = readString(name, section);
            auto vec = readerUnpackMulti(name, section, str, sizeof...(Ts) + 2);
            auto is  = std::make_index_sequence<sizeof...(Ts) + 2>();

            return readTupleImpl<T1, T2, Ts...>(vec, name, section, is);
        }


        SIA remove_space_bounds_if_exists(StrViewCref str) -> StrView;
        SIA remove_brackets_if_exists    (StrViewCref str) -> StrView;

    private:


        // Basic read method
        SIA readString(StrViewCref name, StrViewCref section) -> ftl::String& {
            return cfg_detls::cfgData().getValue(section, name);
        }

        //
        template <typename... Ts, SizeT... _Idx>
        SIA readTupleImpl(StrvVector& vec, StrViewCref name, StrViewCref section, std::index_sequence<_Idx...>) {
            return std::make_tuple(superCast<Ts>(vec[_Idx], name, section)...);
        }

        template <typename T, SizeT... _Idx>
        SIA readArrayImpl(StrvVector& vec, StrViewCref name, StrViewCref section, std::index_sequence<_Idx...>) {
            auto arr = ftl::Array<T, sizeof...(_Idx)>{};

            ((arr[_Idx] = superCast<T>(vec[_Idx], name, section)) , ...);

            return arr;
        }

        template <typename T>
        SIA readVectorImpl(StrvVector& vec, StrViewCref name, StrViewCref section) {
            auto res = ftl::Vector<T>{};

            for (auto& s : vec)
                res.push_back(superCast<T>(s, name, section));

            return res;
        }

        // Numbers
        template <typename T>
        SIA superCast(StrViewCref str, StrViewCref, StrViewCref)
        -> std::enable_if_t<concepts::numbers<T>, T> {
            auto nullTerminatedStr = ftl::String(str);
            return aton<T>(nullTerminatedStr.c_str());
        }

        // String :)
        template <typename T>
        SIA superCast(StrViewCref str, StrViewCref, StrViewCref)
        -> std::enable_if_t<concepts::any_of<T, ftl::String, std::string_view>, T> {
            return str;
        }

        template <typename T>
        SIA superCast(StrViewCref str, StrViewCref name, StrViewCref section)
        -> std::enable_if_t<std::is_same_v<T, bool>, bool> {
            if (str == "true" || str == "on")
                return true;
            else if (str == "false" || str == "off")
                return false;
            else
                RABORTF("Unknown bool value '{}' at key '{}' in section [{}].", str, name, section);
            return false; // !?
        }

        // Array
        template <typename A, typename T = typename A::ValType, SizeT _Size = A::ArraySize>
        SIA superCast(StrViewCref str, StrViewCref name, StrViewCref section)
        -> std::enable_if_t<concepts::any_of<A, ftl::Array<T, _Size>>, A> {
            auto vec = readerUnpackMulti(name, section, str, _Size);
            return readArrayImpl<T>(vec, name, section, std::make_index_sequence<_Size>());
        }

        // Vector
        template <typename A, typename T = typename A::ValType>
        SIA superCast(StrViewCref str, StrViewCref name, StrViewCref section)
        -> std::enable_if_t<concepts::any_of<A, ftl::Vector<T>>, A> {
            auto vec = readerUnpackMulti(name, section, str);
            return readVectorImpl<T>(vec, name, section);
        }

        // Vector2
        template <typename V, typename T = typename V::ValType>
        SIA superCast(StrViewCref str, StrViewCref name, StrViewCref section)
        -> std::enable_if_t<
                concepts::any_of<V, ftl::Vector2<T>, ftl::Vector2Flt<T>>,
                decltype(ftl::Vector2T(std::declval<T>(), std::declval<T>()))>
        {
            auto vec = readerUnpackMulti(name, section, str, 2);
            auto s1 = vec[0];
            auto s2 = vec[1];
            return ftl::Vector2T(
                    superCast<T>(s1, name, section),
                    superCast<T>(s2, name, section));
        }

        // Vector3
        template <typename V, typename T = typename V::ValType>
        SIA superCast(StrViewCref str, StrViewCref name, StrViewCref section)
        -> std::enable_if_t<
                concepts::any_of<V, ftl::Vector3<T>, ftl::Vector3Flt<T>>,
                decltype(ftl::Vector3T(std::declval<T>(), std::declval<T>(), std::declval<T>()))>
        {
            auto vec = readerUnpackMulti(name, section, str, 3);
            auto s1 = vec[0];
            auto s2 = vec[1];
            auto s3 = vec[2];
            return ftl::Vector3T(
                    superCast<T>(s1, name, section),
                    superCast<T>(s2, name, section),
                    superCast<T>(s3, name, section));
        }

        static auto readerUnpackMulti
        (StrViewCref name, StrViewCref section, StrViewCref str, SizeT required = 0) -> StrvVector;

        void load();


        // Singleton impl
    public:
        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator= (const ConfigManager&) = delete;

        static ConfigManager& instance() {
            static ConfigManager inst;
            return inst;
        }

    private:
        ConfigManager () {
            cfg_detls::cfg_state()._onCreate  = true;
            load();
            cfg_detls::cfg_state()._onCreate  = false;
            cfg_detls::cfg_state()._isCreated = true;
        }
        ~ConfigManager() = default;
    };



    namespace cfg {
        using StrViewCref = const std::string_view&;

        /**
         * Add entry file to loading
         * @param path - path to config entry file
         */
        inline void addCfgEntry(ftl::String& path) {
            cfg_detls::cfg_state().addCfgEntry(path);
        }

        /**
         * Remove all entry files
         */
        inline void resetCfgEntries() {
            cfg_detls::cfg_state().clearCfgEntries();
        }

        /**
         * Get instance of ConfigManager
         * @return reference to ConfigManager
         */
        inline ConfigManager& cfg() { return ConfigManager::instance(); }

        /**
         * Basic config read
         * @tparam Ts - type (types) of value
         * @param name - value name
         * @param section - section name. If unused - perform read from global namespace
         * @return Value with T type, or tuple if more one types passed
         */
        template<typename... Ts>
        IA read(StrViewCref name, StrViewCref section = cfg_detls::GLOBAL_NAMESPACE) {
            return cfg().read<Ts...>(name, section);
        }

        ///////////// READ IF SECTION OR VALUE EXISTS //////////////

        /**
         * Basic config read from global namespace if value or section exists
         * @tparam T - type of value
         * @param name - value name
         * @param def_val - default value with T type
         * @return Value with T type or default val
         */
        template <typename T>
        IA read_ie(StrViewCref name, const T& def_val) {
            return cfg().read_ie<T>(name, def_val);
        }
        /**
         * Basic config read from global namespace if value or section exists
         * @tparam T1, T2, Ts - types of value
         * @param name - value name
         * @param def_vals - default value tuple with T1, T2, Ts types
         * @return Tuple of T1, T2, Ts or default tuple
         */
        template <typename T1, typename T2, typename... Ts>
        IA read_ie(StrViewCref name, const std::tuple<T1, T2, Ts...>& def_vals) {
            return cfg().read_ie<T1, T2, Ts...>(name, def_vals);
        }
        /**
         * Basic config read if value or section exists
         * @tparam T - type of value
         * @param name - value name
         * @param section - section name
         * @param def_val - default value with T type
         * @return Value with T type or default val
         */
        template <typename T>
        IA read_ie(StrViewCref name, StrViewCref section, const T& def_val) {
            return cfg().read_ie<T>(name, section, def_val);
        }
        /**
         * Basic config read if value or section exists
         * @tparam T1, T2, Ts - types of value
         * @param name - value name
         * @param section - section name
         * @param def_vals - default value tuple with T1, T2, Ts types
         * @return Tuple of T1, T2, Ts or default tuple
         */
        template <typename T1, typename T2, typename... Ts>
        IA read_ie(StrViewCref name, StrViewCref section, const std::tuple<T1, T2, Ts...>& def_vals) {
            return cfg().read_ie<T1, T2, Ts...>(name, section, def_vals);
        }

        ///////////// READ IF VALUE EXISTS //////////////

        /**
         * Basic config read from global namespace if value exists (Assert if section does't exist)
         * @tparam T - type of value
         * @param name - value name
         * @param def_val - default value with T type
         * @return Value with T type or default val
         */
        template <typename T>
        IA read_ive(StrViewCref name, const T& def_val) {
            return cfg().read_ive<T>(name, def_val);
        }
        /**
         * Basic config read from global namespace if value exists (Assert if section does't exist)
         * @tparam T1, T2, Ts - types of value
         * @param name - value name
         * @param def_vals - default value tuple with T1, T2, Ts types
         * @return Tuple of T1, T2, Ts or default tuple
         */
        template <typename T1, typename T2, typename... Ts>
        IA read_ive(StrViewCref name, const std::tuple<T1, T2, Ts...>& def_vals) {
            return cfg().read_ive<T1, T2, Ts...>(name, def_vals);
        }
        /**
         * Basic config read if value exists (Assert if section does't exist)
         * @tparam T - type of value
         * @param name - value name
         * @param section - section name
         * @param def_val - default value with T type
         * @return Value with T type or default val
         */
        template <typename T>
        IA read_ive(StrViewCref name, StrViewCref section, const T& def_val) {
            return cfg().read_ive<T>(name, section, def_val);
        }
        /**
         * Basic config read if value exists (Assert if section does't exist)
         * @tparam T1, T2, Ts - types of value
         * @param name - value name
         * @param section - section name
         * @param def_vals - default value tuple with T1, T2, Ts types
         * @return Tuple of T1, T2, Ts or default tuple
         */
        template <typename T1, typename T2, typename... Ts>
        IA read_ive(StrViewCref name, StrViewCref section, const std::tuple<T1, T2, Ts...>& def_vals) {
            return cfg().read_ive<T1, T2, Ts...>(name, section, def_vals);
        }



        ////////////// FORCE READ FUNCTIONS ///////////////////
        ///////////// READ IF SECTION OR VALUE EXISTS //////////////

        /**
         * Read from global namespace if value or section exists
         * Read cfg value without full loading (use for prevent recursive ConfigManager initialization)
         * @tparam T - type of value
         * @param name - value name
         * @param def_val - default value with T type
         * @return Value with T type or default val
         */
        template <typename T>
        IA force_read_ie(StrViewCref name, const T& def_val) {
            if (cfg_detls::cfg_state().onCreate())
                return ConfigManager::force_read_ie<T>(name, def_val);
            else
                return read_ie<T>(name, def_val);
        }
        /**
         * Read from global namespace if value or section exists
         * Read cfg value without full loading (use for prevent recursive ConfigManager initialization)
         * @tparam T1, T2, Ts - types of value
         * @param name - value name
         * @param def_vals - default value tuple with T1, T2, Ts types
         * @return Tuple of T1, T2, Ts or default tuple
         */
        template <typename T1, typename T2, typename... Ts>
        IA force_read_ie(StrViewCref name, const std::tuple<T1, T2, Ts...>& def_vals) {
            if (cfg_detls::cfg_state().onCreate())
                return ConfigManager::force_read_ie<T1, T2, Ts...>(name, def_vals);
            else
                return read_ie<T1, T2, Ts...>(name, def_vals);
        }
        /**
         * Read if value or section exists
         * Read cfg value without full loading (use for prevent recursive ConfigManager initialization)
         * @tparam T - type of value
         * @param name - value name
         * @param section - section name
         * @param def_val - default value with T type
         * @return Value with T type or default val
         */
        template <typename T>
        IA force_read_ie(StrViewCref name, StrViewCref section, const T& def_val) {
            if (cfg_detls::cfg_state().onCreate())
                return ConfigManager::force_read_ie<T>(name, section, def_val);
            else
                return read_ie<T>(name, section, def_val);
        }
        /**
         * Read if value or section exists
         * Read cfg value without full loading (use for prevent recursive ConfigManager initialization)
         * @tparam T1, T2, Ts - types of value
         * @param name - value name
         * @param section - section name
         * @param def_vals - default value tuple with T1, T2, Ts types
         * @return Tuple of T1, T2, Ts or default tuple
         */
        template <typename T1, typename T2, typename... Ts>
        IA force_read_ie(StrViewCref name, StrViewCref section, const std::tuple<T1, T2, Ts...>& def_vals) {
            if (cfg_detls::cfg_state().onCreate())
                return ConfigManager::force_read_ie<T1, T2, Ts...>(name, section, def_vals);
            else
                return read_ie<T1, T2, Ts...>(name, section, def_vals);
        }

        ///////////// READ IF VALUE EXISTS //////////////

        /**
         * Read from global namespace if value exists (Assert if section does't exist)
         * Read cfg value without full loading (use for prevent recursive ConfigManager initialization)
         * @tparam T - type of value
         * @param name - value name
         * @param def_val - default value with T type
         * @return Value with T type or default val
         */
        template <typename T>
        IA force_read_ive(StrViewCref name, const T& def_val) {
            if (cfg_detls::cfg_state().onCreate())
                return ConfigManager::force_read_ive<T>(name, def_val);
            else
                return read_ive<T>(name, def_val);
        }
        /**
         * Read from global namespace if value exists (Assert if section does't exist)
         * Read cfg value without full loading (use for prevent recursive ConfigManager initialization)
         * @tparam T1, T2, Ts - types of value
         * @param name - value name
         * @param def_vals - default value tuple with T1, T2, Ts types
         * @return Tuple of T1, T2, Ts or default tuple
         */
        template <typename T1, typename T2, typename... Ts>
        IA force_read_ive(StrViewCref name, const std::tuple<T1, T2, Ts...>& def_vals) {
            if (cfg_detls::cfg_state().onCreate())
                return ConfigManager::force_read_ive<T1, T2, Ts...>(name, def_vals);
            else
                return read_ive<T1, T2, Ts...>(name, def_vals);
        }
        /**
         * Read if value exists (Assert if section does't exist)
         * Read cfg value without full loading (use for prevent recursive ConfigManager initialization)
         * @tparam T - type of value
         * @param name - value name
         * @param section - section name
         * @param def_val - default value with T type
         * @return Value with T type or default val
         */
        template <typename T>
        IA force_read_ive(StrViewCref name, StrViewCref section, const T& def_val) {
            if (cfg_detls::cfg_state().onCreate())
                return ConfigManager::force_read_ive<T>(name, section, def_val);
            else
                return read_ive<T>(name, section, def_val);
        }
        /**
         * Read if value exists (Assert if section does't exist)
         * Read cfg value without full loading (use for prevent recursive ConfigManager initialization)
         * @tparam T1, T2, Ts - types of value
         * @param name - value name
         * @param section - section name
         * @param def_vals - default value tuple with T1, T2, Ts types
         * @return Tuple of T1, T2, Ts or default tuple
         */
        template <typename T1, typename T2, typename... Ts>
        IA force_read_ive(StrViewCref name, StrViewCref section, const std::tuple<T1, T2, Ts...>& def_vals) {
            if (cfg_detls::cfg_state().onCreate())
                return ConfigManager::force_read_ive<T1, T2, Ts...>(name, section, def_vals);
            else
                return read_ive<T1, T2, Ts...>(name, section, def_vals);
        }

        /**
        * Read cfg value without full loading (use for prevent recursive ConfigManager initialization)
        * @tparam Ts - type (types) of value
        * @param name - value name
        * @param section - section name. If unused - perform read from global namespace
        * @return Value with T type, or tuple if more one types passed
        */
        template<typename... Ts>
        IA force_read(
                const std::string_view &name,
                const std::string_view &section = cfg_detls::GLOBAL_NAMESPACE) {
            if (cfg_detls::cfg_state().onCreate())
                return ConfigManager::force_read<Ts...>(name, section);
            else
                return read<Ts...>(name, section);
        }
    }

} // namespace base


#undef IA // inline auto
#undef SIA // static inline auto