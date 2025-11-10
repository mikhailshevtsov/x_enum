#ifndef X_ENUM_HPP
#define X_ENUM_HPP

#include <type_traits>
#include <array>
#include <stdexcept>
#include <string_view>

#define X_FOREACH(ENUM_NAME) X_ENUM_##ENUM_NAME

#define X_DEFINE_ENUMER(NAME, ...) NAME __VA_OPT__(=) __VA_ARGS__,
#define X_ENUMER(NAME, ...) enum_type::NAME,
#define X_ENUMER_NAME(NAME, ...) #NAME,
#define X_ENUMER_VALUE(NAME, ...) static_cast<underlying_type>(enum_type::NAME),

#define X_DEFINE_ENUM_IMPL(ENUM_NAME, SCOPE, ...) enum SCOPE ENUM_NAME __VA_OPT__(:) __VA_ARGS__ { X_FOREACH(ENUM_NAME)(X_DEFINE_ENUMER) }
#define X_DEFINE_ENUM(ENUM_NAME, ...) X_DEFINE_ENUM_IMPL(ENUM_NAME, class, __VA_ARGS__)
#define X_DEFINE_ENUM_UNSCOPED(ENUM_NAME, ...) X_DEFINE_ENUM_IMPL(ENUM_NAME, , __VA_ARGS__)

#define X_DESCRIBE_ENUM_BODY(ENUM_NAME) \
    using underlying_type = std::underlying_type_t<enum_type>; \
    \
private: \
    static constexpr enum_type _enumers[] { X_FOREACH(ENUM_NAME)(X_ENUMER) }; \
    static constexpr const char* _names[] { X_FOREACH(ENUM_NAME)(X_ENUMER_NAME) }; \
    static constexpr underlying_type _values[] { X_FOREACH(ENUM_NAME)(X_ENUMER_VALUE) }; \
    \
public: \
    static constexpr const char* name() noexcept { return #ENUM_NAME; } \
    static constexpr const auto& enumers() noexcept { return _enumers; } \
    static constexpr const auto& names() noexcept { return _names; } \
    static constexpr const auto& values() noexcept { return _values; } \
    static constexpr std::size_t size() noexcept { return sizeof(_names) / sizeof(const char*); } \

#define X_ENUM_TRAITS(ENUM_NAME) X_##ENUM_NAME##_traits

#define X_DESCRIBE_ENUM_IMPL(ENUM_NAME, IN_CLASS) \
class X_ENUM_TRAITS(ENUM_NAME) \
{ \
public: \
    using enum_type = ENUM_NAME; \
    X_DESCRIBE_ENUM_BODY(ENUM_NAME) \
}; \
IN_CLASS constexpr X_ENUM_TRAITS(ENUM_NAME) x_enum_traits(std::type_identity_t<ENUM_NAME>) noexcept { return {}; } \
static_assert(x_enum::is_enum_v<ENUM_NAME>, "using X_ENUM inside the class scope");

#define X_DESCRIBE_ENUM(ENUM_NAME) X_DESCRIBE_ENUM_IMPL(ENUM_NAME,)
#define X_DESCRIBE_ENUM_IN_CLASS(ENUM_NAME) X_DESCRIBE_ENUM_IMPL(ENUM_NAME, friend)

#define X_ENUM(ENUM_NAME, ...) \
X_DEFINE_ENUM(ENUM_NAME, __VA_ARGS__); \
X_DESCRIBE_ENUM(ENUM_NAME)

#define X_ENUM_IN_CLASS(ENUM_NAME, ...) \
X_DEFINE_ENUM(ENUM_NAME, __VA_ARGS__); \
X_DESCRIBE_ENUM_IN_CLASS(ENUM_NAME)

#define X_ENUM_UNSCOPED(ENUM_NAME, ...) \
X_DEFINE_ENUM_UNSCOPED(ENUM_NAME, __VA_ARGS__); \
X_DESCRIBE_ENUM(ENUM_NAME)

#define X_ENUM_UNSCOPED_IN_CLASS(ENUM_NAME, ...) \
X_DEFINE_ENUM_UNSCOPED(ENUM_NAME, __VA_ARGS__); \
X_DESCRIBE_ENUM_IN_CLASS(ENUM_NAME)

#define X_DESCRIBE_ENUM_EXTERNAL(ENUM_NAME, ...) \
template <typename T> \
struct x_enum::is_enum<__VA_ARGS__::ENUM_NAME, T> : std::true_type {}; \
\
template <> \
class x_enum::enum_traits<__VA_ARGS__::ENUM_NAME> \
{ \
public: \
    using enum_type = __VA_ARGS__::ENUM_NAME; \
    X_DESCRIBE_ENUM_BODY(ENUM_NAME) \
};

namespace x_enum
{

template <typename...>
using void_t = void;

template <typename, typename = void>
struct is_enum : std::false_type {};

template <typename T>
struct is_enum<T, void_t<decltype(x_enum_traits(std::type_identity_t<T>{}))>> : std::true_type {};

template <typename T>
constexpr bool is_enum_v = is_enum<T>::value;

template <typename T>
concept enumeration = is_enum_v<T>;

template <enumeration EnumT>
struct enum_traits : decltype(x_enum_traits(std::type_identity_t<EnumT>{})) {};

template <enumeration EnumT>
using underlying_type_t = typename enum_traits<EnumT>::underlying_type;

template <enumeration EnumT>
constexpr const char* enum_name() noexcept { return enum_traits<EnumT>::name(); }

template <enumeration EnumT>
constexpr const auto& enum_enumers() noexcept { return enum_traits<EnumT>::enumers(); }

template <enumeration EnumT>
constexpr const auto& enum_names() noexcept { return enum_traits<EnumT>::names(); }

template <enumeration EnumT>
constexpr const auto& enum_values() noexcept { return enum_traits<EnumT>::values(); }

template <enumeration EnumT>
constexpr std::size_t enum_size() noexcept { return enum_traits<EnumT>::size(); }

template <enumeration EnumT>
constexpr const char* to_string(EnumT enumer)
{
    constexpr const auto& enumers = enum_enumers<EnumT>();
    constexpr std::size_t size = enum_size<EnumT>();
    for (std::size_t i = 0; i < size; ++i)
        if (enumers[i] == enumer)
            return enum_names<EnumT>()[i];
    throw std::out_of_range("enum value is out of range");
}

template <enumeration EnumT>
constexpr EnumT from_string(std::string_view name)
{
    constexpr const auto& names = enum_names<EnumT>();
    constexpr std::size_t size = enum_size<EnumT>();
    for (std::size_t i = 0; i < size; ++i)
        if (names[i] == name)
            return enum_enumers<EnumT>()[i];
    throw std::out_of_range("enum index is out of range");
}

}

#endif //X_ENUM_HPP