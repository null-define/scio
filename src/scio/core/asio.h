#ifndef scio_FBCBFA29_B635_4C06_A3D6_6127BAA5C5A6
#define scio_FBCBFA29_B635_4C06_A3D6_6127BAA5C5A6

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <functional>

namespace scio {
namespace net = boost::asio;
using net::awaitable;
using net::co_spawn;
using net::detached;
using net::signal_set;
using net::use_awaitable;
using net::use_awaitable_t;
using net::experimental::as_tuple_t;
namespace this_coro = net::this_coro;

using default_token = net::experimental::as_tuple_t<use_awaitable_t<>>;

/**
 * @brief run task in a new coroutine
 * @tparam Executor
 * @tparam Awaitable
 * @param  executor         executor to run task
 * @param  f                task to run
 */
template <typename Executor, typename Awaitable>
void co_run_task(Executor executor, Awaitable &&f) {
  co_spawn(executor, std::forward<decltype(f)>(f), detached);
};

/**
 * @brief spawn task in a main coroutine
 * @tparam A
 * @param  a                task to run
 * @return int
 */
template <typename A> int execute_main(A &&a) {
  net::io_context io_context(1);
  net::signal_set signals(io_context, SIGINT, SIGTERM);
  signals.async_wait([&](auto, auto) { io_context.stop(); });
  co_spawn(io_context, std::move(a), detached);
  io_context.run();
  return 0;
}
} // namespace scio

#endif /* scio_FBCBFA29_B635_4C06_A3D6_6127BAA5C5A6 */
