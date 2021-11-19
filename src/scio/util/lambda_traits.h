#ifndef scio_AFEA9930_5C14_4301_9454_8A278DBAAA9F
#define scio_AFEA9930_5C14_4301_9454_8A278DBAAA9F
#include <cstdint>
#include <tuple>
namespace scio {

namespace detail {
template <typename R, typename... Args> struct lambda_traits_base_noconst {
  using result_type = R;
  using args_type = std::tuple<Args...>;
  template <size_t idx>
  using arg_type_at = std::tuple_element_t<idx, args_type>;
  static constexpr size_t arity = sizeof...(Args);
};

template <typename T> struct lambda_traits_base;

template <typename R, typename Cls, typename... Args>
struct lambda_traits_base<R (Cls::*)(Args...)>
    : lambda_traits_base_noconst<R, Args...> {
  static constexpr bool is_const = false;
// noexcept is part of function signature since C++17
#if __cplusplus >= 201703L
  static constexpr bool is_noexcept = false;
#endif
};

template <typename R, typename Cls, typename... Args>
struct lambda_traits_base<R (Cls::*)(Args...) const>
    : lambda_traits_base_noconst<R, Args...> {
  static constexpr bool is_const = true;
#if __cplusplus >= 201703L
  static constexpr bool is_noexcept = false;
#endif
};

#if __cplusplus >= 201703L
template <typename R, typename Cls, typename... Args>
struct lambda_traits_base<R (Cls::*)(Args...) noexcept>
    : lambda_traits_base_noconst<R, Args...> {
  static constexpr bool is_const = false;
  static constexpr bool is_noexcept = true;
};

template <typename R, typename Cls, typename... Args>
struct lambda_traits_base<R (Cls::*)(Args...) const noexcept>
    : lambda_traits_base_noconst<R, Args...> {
  static constexpr bool is_const = true;
  static constexpr bool is_noexcept = true;
};
#endif

template <class T, class... TArgs> struct is_lambda_base {
private:
  template <class F>
  static auto test(decltype(&F::operator())) -> std::true_type;
  template <class F, class... FTArgs>
  static auto test(decltype(&F::template operator()<FTArgs...>))
      -> std::true_type;
  template <class F> static auto test(...) -> std::false_type;

public:
  static constexpr bool value = decltype(test<T, TArgs...>(0))::value;
};

template <typename T> struct function_traits_base;

template <typename R, typename... Args>
struct function_traits_base<R(Args...)>
    : lambda_traits_base_noconst<R, Args...> {
#if __cplusplus >= 201703L
  static constexpr bool is_noexcept = false;
#endif
};

#if __cplusplus >= 201703L
template <typename R, typename... Args>
struct function_traits_base<R(Args...) noexcept>
    : lambda_traits_base_noconst<R, Args...> {
  static constexpr bool is_noexcept = true;
};
#endif

template <typename T> struct is_function_base {
private:
  template <typename F>
  static auto test(typename function_traits_base<F>::result_type *)
      -> std::true_type;
  template <typename F> static auto test(...) -> std::false_type;

public:
  static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename T, typename... TArgs> struct is_convertible_to_fp_base {
private:
  template <typename F, typename R, typename... Args>
  static auto test0(decltype(static_cast<R (*)(Args...)>(std::declval<F>())))
      -> std::true_type;

  template <typename F, typename Trait, size_t... i>
  static auto test1(std::index_sequence<i...>)
      -> decltype(test0<F, typename Trait::result_type,
                        typename Trait::template arg_type_at<i>...>(0));

  template <typename F,
            typename Trait = lambda_traits_base<decltype(&F::operator())>>
  static auto test(std::int64_t)
      -> decltype(test1<F, Trait>(std::make_index_sequence<Trait::arity>()));
  template <typename F, typename... FTArgs,
            typename Trait =
                lambda_traits_base<decltype(&F::template operator()<TArgs...>)>>
  static auto test(std::uint64_t)
      -> decltype(test1<F, Trait>(std::make_index_sequence<Trait::arity>()));

  template <typename> static auto test(...) -> std::false_type;

public:
  static constexpr bool value =
      is_function_base<std::remove_pointer_t<T>>::value ||
      decltype(test<T, TArgs...>(0))::value;
};
} // namespace detail

template <typename...> struct lambda_traits;

template <typename F>
struct lambda_traits<F> : detail::lambda_traits_base<decltype(&F::operator())> {
};

template <typename F, typename... TArgs>
struct lambda_traits<F, TArgs...>
    : detail::lambda_traits_base<decltype(&F::template operator()<TArgs...>)> {
};

template <typename F, typename... TArgs>
struct is_lambda : detail::is_lambda_base<F, TArgs...> {};

template <typename F>
struct function_traits : detail::function_traits_base<F> {};

template <typename F> struct is_function : detail::is_function_base<F> {};

template <typename F, typename... TArgs>
struct is_convertible_to_fp : detail::is_convertible_to_fp_base<F, TArgs...> {};

} // namespace scio
#endif /* scio_AFEA9930_5C14_4301_9454_8A278DBAAA9F */
