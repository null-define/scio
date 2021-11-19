#ifndef scio_D0278647_E35A_4F50_ADA4_EE979FDC7648
#define scio_D0278647_E35A_4F50_ADA4_EE979FDC7648

#include <cstdio>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/printf.h>
#include <iostream>
#include <scio/core/asio.h>
#include <scio/core/util.h>
#include <string>

namespace scio {

namespace console {
/**
 * @brief (lambda function) print , a wrapper for fmt::print
 * @example print(stdout, "hello {}", "world");
 **/
constexpr auto print = [](auto &&dest, auto &&msg,
                          auto &&...args) -> awaitable<void> {
  co_return fmt::print(dest, std::forward<decltype(msg)>(msg),
                       std::forward<decltype(args)>(args)...);
};

/**
 * @brief print to stdout
 */
constexpr auto log = bind_first_arg(print, std::cout);

/**
 * @brief print to stderr
 */
constexpr auto err = bind_first_arg(print, std::cerr);

/**
 * @brief read from stdin
 */
constexpr auto read = []() -> awaitable<std::string> {
  std::string line;
  std::getline(std::cin, line);
  co_return line;
};

} // namespace console

} // namespace scio

#endif /* scio_D0278647_E35A_4F50_ADA4_EE979FDC7648 */
