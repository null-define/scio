#ifndef scio_C3909B47_BEF4_4485_BA97_0620EA44A949
#define scio_C3909B47_BEF4_4485_BA97_0620EA44A949

#include <exception>
#include <execinfo.h>
#include <fcntl.h>
#include <functional>
#include <string_view>
#include <system_error>
#include <time.h>
#include <unistd.h>
#include <utility>

namespace scio {
/**
 * @brief guard for operations
 * @tparam Fn
 */
template <typename Fn> struct on_scope_exit {
  on_scope_exit(Fn &&fn) : _fn(std::move(fn)) {}
  ~on_scope_exit() { this->_fn(); }

private:
  Fn _fn;
};

/** Convert errno to exception
 * @throw std::runtime_error / std::system_error
 * @return never
 */
[[noreturn]] void panic(std::string_view sv, int err) {
#ifndef NDEBUG
  // https://stackoverflow.com/questions/77005/how-to-automatically-generate-a-stacktrace-when-my-program-crashes
  void *array[32];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 32);

  // print out all the frames to stderr
  fprintf(stderr, "Error: errno %d:\n", err);
  backtrace_symbols_fd(array, size, STDERR_FILENO);

  // __asm__("int $3");
#endif
  throw std::system_error(err, std::generic_category(), sv.data());
}

struct panic_on_err {
  panic_on_err(std::string_view _command, bool _use_errno)
      : command(_command), use_errno(_use_errno) {}
  std::string_view command;
  bool use_errno;
};

inline int operator|(int ret, panic_on_err &&poe) {
  if (ret < 0) {
    if (poe.use_errno) {
      panic(poe.command, errno);
    } else {
      if (ret != -ETIME)
        panic(poe.command, -ret);
    }
  }
  return ret;
}

/**
 * @brief bind first parameter with given argument
 * @tparam F
 * @tparam T
 * @param  f                function object
 * @param  arg_first        first argument
 * @return constexpr auto
 */
template <typename F, typename T>
constexpr auto bind_first_arg(F &&f, T &&arg_first) {
  return [&](auto &&...args) {
    return std::forward<F>(f)(std::forward<T>(arg_first),
                              std::forward<decltype(args)>(args)...);
  };
}

} // namespace scio

#endif /* scio_C3909B47_BEF4_4485_BA97_0620EA44A949 */
