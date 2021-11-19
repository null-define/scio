#ifndef scio_E335F82B_6FD4_4D7E_B662_79F9DD7A3C67
#define scio_E335F82B_6FD4_4D7E_B662_79F9DD7A3C67

#include <cstdint>
#include <exception>
#include <functional>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace scio {

namespace procedure {

/**
 * @brief procedure_t with overloads operator >> provides a way to pass
 * procedure_t object to other procedure_t object
 */
template <typename Fn, typename... Args> struct procedure_t {

  /**
   * @brief Construct a new procedure_t object
   * @param  u                function object
   */
  explicit procedure_t(const Fn &f) : fn_(f) {}
  explicit procedure_t(Fn &&f) : fn_(std::move(f)) {}

  template <typename... ArgsF> auto run(ArgsF &&...args) const {
    return fn_(std::move(args)...);
  }

  auto get_fn() const { return fn_; }

  template <typename F> auto operator>>(F &&fn2) {

    auto fn = [fn_1 = std::move(fn_), fn_2 = std::move(fn2)](auto &&...args) {
      return fn_2(fn_1(std::forward<decltype(args)>(args)...));
    };
    return procedure_t<decltype(fn), Args...>(std::move(fn));
  }

private:
  Fn fn_;
};

template <typename... Args> constexpr auto make(auto &&fn) {
  return procedure_t<decltype(fn), Args...>(std::move(fn));
};

constexpr auto make(auto &fn) {
  return procedure_t<decltype(fn)>(std::move(fn));
};
} // namespace procedure

} // namespace scio

#endif /* scio_E335F82B_6FD4_4D7E_B662_79F9DD7A3C67 */
