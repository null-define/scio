#ifndef scio_B4FCA0AA_258E_4C37_86D9_0A049A87987D
#define scio_B4FCA0AA_258E_4C37_86D9_0A049A87987D

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <optional>
#include <scio/core/asio.h>
#include <scio/core/beast.h>
#include <scio/core/result.h>
#include <scio/core/tcp.h>
#include <scio/util/lambda_traits.h>

namespace scio {
namespace http {

struct client {
  /**
   * @brief Construct a new client object
   * @param  ioe              io executor
   * @param  host             host
   * @param  port             port
   */
  client(const net::any_io_executor &ioe, std::string host,
         std::string port = "80")
      : ioe_(ioe), host_(host), port_(port), stream_(ioe_) {}

  /**
   * @brief Construct a new client object
   * @param  ioe              io executor
   */
  client(const net::any_io_executor &ioe) : ioe_(ioe), stream_(ioe_) {}

  /**
   * @brief connect to server
   * @param  host            host
   * @param  port            port
   * @return awaitable<scio::result_t<net::ip::tcp::endpoint>>
   */
  auto connect(std::string host, std::string port = "80")
      -> awaitable<scio::result_t<net::ip::tcp::endpoint>> {
    host_ = host;
    port_ = port;
    co_return co_await this->connect();
  }

  /**
   * @brief connect to server
   * @return awaitable<scio::result_t<net::ip::tcp::endpoint>>
   */
  auto connect() -> awaitable<scio::result_t<net::ip::tcp::endpoint>> {
    tcp::resolver resolver(ioe_);
    tcp::resolver::results_type endpoints = resolver.resolve(host_, port_);
    stream_.expires_after(std::chrono::seconds(30));
    co_return co_await stream_.async_connect(endpoints);
  }

  /**
   * @brief send request asynchronously
   * @param  target           target
   * @param  req              request
   * @param  time_out         time out
   * @return awaitable<
   * scio::result_t<beast::http::response<beast::http::dynamic_body>>>
   */
  auto send_request(const std::string &target,
                    const beast::http::request<beast::http::string_body> &req,
                    const std::chrono::steady_clock::duration &time_out)
      -> awaitable<
          scio::result_t<beast::http::response<beast::http::dynamic_body>>> {
    stream_.expires_after(time_out);
    auto [ec_w, n_w] = co_await beast::http::async_write(stream_, req);
    if (ec_w) {
      co_return ec_w;
    }
    beast::flat_buffer b;
    beast::http::response<beast::http::dynamic_body> res;
    stream_.expires_after(time_out);
    auto [ec_r, n] = co_await beast::http::async_read(stream_, b, res);
    if (ec_r) {
      co_return ec_r;
    }
    co_return res;
  }

  /**
   * @brief send request
   * @tparam Request
   * @param  target           target
   * @param  req              request
   * @param  os               output stream
   * @param  time_out         time out
   * @return awaitable<scio::result_t<void>>
   */
  template <typename Request>
  auto send_request(std::string target, Request req, std::ostream &os,
                    std::chrono::steady_clock::duration time_out)
      -> awaitable<scio::result_t<void>> {
    stream_.expires_after(time_out);
    auto [ec_w, n_w] = co_await beast::http::async_write(stream_, req);
    if (ec_w) {
      co_return ec_w;
    }
    beast::flat_buffer b;
    beast::http::response_parser<beast::http::buffer_body> p;
    auto [ec_rh, n_h] = co_await async_read_header(stream_, b, p);
    if (ec_rh)
      co_return ec_rh;
    while (!p.is_done()) {
      char buf[512];
      p.get().body().data = buf;
      p.get().body().size = sizeof(buf);
      auto [ec_r, n2] = co_await async_read(stream_, b, p);
      if (ec_r == boost::beast::http::error::need_buffer) {
        ec_r = {};
      }
      if (ec_r)
        co_return ec_r;
      os.write(buf, sizeof(buf) - p.get().body().size);
    }
    co_return result_nullopt_t{};
  }

  /**
   * @brief http post
   * @param  target           target
   * @param  body             request body
   * @param  time_out         time out
   * @param  version          version
   * @return awaitable<
   * scio::result_t<beast::http::response<beast::http::dynamic_body>>>
   */
  auto http_post(
      std::string target, std::string body,
      std::chrono::steady_clock::duration time_out = std::chrono::seconds(30),
      int version = 11)
      -> awaitable<
          scio::result_t<beast::http::response<beast::http::dynamic_body>>> {
    auto req = make_http_string_body_header(target);
    req.method(beast::http::verb::post);
    req.set(beast::http::field::content_length, std::to_string(body.length()));
    req.set(beast::http::field::body, body);
    co_return co_await this->send_request(target, req, time_out);
  }

  /**
   * @brief http post
   * @param  target           target
   * @param  body             request body
   * @param  os               output stream
   * @param  time_out         time out
   * @param  version          version
   * @return awaitable<scio::result_t<void>>
   */
  auto http_post(
      std::string target, std::string body, std::ostream &os,
      std::chrono::steady_clock::duration time_out = std::chrono::seconds(30),
      int version = 11) -> awaitable<scio::result_t<void>> {
    auto req = make_http_string_body_header(target);
    req.method(beast::http::verb::post);
    req.set(beast::http::field::content_length, std::to_string(body.length()));
    req.set(beast::http::field::body, body);
    co_return co_await this->send_request(target, req, os, time_out);
  }

  /**
   * @brief  http get
   * @param  target           target
   * @param  time_out         time out
   * @return awaitable<
   * scio::result_t<beast::http::response<beast::http::dynamic_body>>>
   */
  auto http_get(
      std::string target,
      std::chrono::steady_clock::duration time_out = std::chrono::seconds(30))
      -> awaitable<
          scio::result_t<beast::http::response<beast::http::dynamic_body>>> {
    auto req = make_http_string_body_header(target);
    req.method(beast::http::verb::get);
    co_return co_await this->send_request(target, req, time_out);
  }

  /**
   * @brief http get
   * @param  target           target
   * @param  os               output stream
   * @param  time_out         time out
   * @return awaitable<scio::result_t<void>>
   */
  auto http_get(
      std::string target, std::ostream &os,
      std::chrono::steady_clock::duration time_out = std::chrono::seconds(30))
      -> awaitable<scio::result_t<void>> {
    auto req = make_http_string_body_header(target);
    req.method(beast::http::verb::get);
    co_return co_await this->send_request(target, req, os, time_out);
  }

  void close() { stream_.close(); }

protected:
  auto make_http_string_body_header(std::string target)
      -> beast::http::request<beast::http::string_body> {
    beast::http::request<beast::http::string_body> req{};
    req.version(11);
    req.target(target);
    req.set(beast::http::field::host, host_);
    req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    return req;
  }

private:
  net::any_io_executor ioe_;
  std::string host_;
  std::string port_;
  tcp::stream stream_;
};

} // namespace http
} // namespace scio

#endif /* scio_B4FCA0AA_258E_4C37_86D9_0A049A87987D */
