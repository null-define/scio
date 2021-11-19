//------------------------------------------------------------------------------
//
// Example: HTTP client, coroutine
//
//------------------------------------------------------------------------------

#include <iostream>

#include <scio/io/console.h>
#include <scio/io/filesystem.h>
#include <scio/io/http.h>

using namespace scio;

//------------------------------------------------------------------------------

// Report a failure
awaitable<void> fail(beast::error_code ec, char const *what) {
  co_await console::err("error while {}, msg:{}\n", what, ec.message());
}

// Performs an HTTP GET and prints the response
awaitable<void> do_session() {
  auto executor = co_await this_coro::executor;
  auto c = http::client(executor);
  on_scope_exit http_client_guard([&]() { c.close(); });
  /**
   * @brief connect to server
   */
  auto r_c = co_await c.connect("www.baidu.com");

  /**
   * @brief easy to get the response
   */
  auto r_g = co_await c.http_get("/");
  if (r_g)
    co_run_task(executor, filesystem::write(
                         "/path/to/1.html",
                         beast::buffers_to_string(r_g.value().body().data())));
  else
    co_await fail(r_g.error(), "HTTP GET");

  /**
   * @brief stream way to get the response
   */
  std::ofstream file("/path/to/2.html");
  on_scope_exit file_guard([&]() { file.close(); });
  auto f = co_await c.http_get("/", file);
  if (!f)
    co_await fail(f.error(), "HTTP GET");
  file << std::endl;
  co_return;
}

//------------------------------------------------------------------------------

int main(int argc, char **argv) {
  execute_main(do_session);
  return 0;
}