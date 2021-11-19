#ifndef scio_C8E90AED_6BBD_46A3_A06D_BA54EB9E7B83
#define scio_C8E90AED_6BBD_46A3_A06D_BA54EB9E7B83

#include <boost/asio/error.hpp>
#include <optional>
#include <tuple>

namespace scio {
/**
 * @brief type for result of boost asio asynchronous operation
 * @tparam T 
 */
template <typename T> struct result_t {
  result_t() = default;
  result_t(const result_t &) = default;
  result_t(result_t &&) = default;

  result_t(const std::tuple<boost::system::error_code, T> &ev) {
    if (std::get<0>(ev)) {
      ec_ = std::get<0>(ev);
    } else {
      value_ = std::get<1>(ev);
    }
  }

  result_t(T &&value) : value_(std::move(value)) {}
  result_t(boost::system::error_code &&ec) : ec_(ec) {}
  result_t(const boost::system::error_code &ec) : ec_(ec) {}
  const T &value() const { return value_.value(); }
  const boost::system::error_code &error() const { return ec_; }

  bool has_value() const { return value_.has_value(); }

  operator bool() const { return ec_ ? false : true; }

private:
  boost::system::error_code ec_;
  std::optional<T> value_;
};

template <> struct result_t<void> {
  result_t() = default;
  result_t(const result_t &) = default;
  result_t(result_t &&) = default;

  result_t(boost::system::error_code &&ec) : ec_(ec) {}
  result_t(const boost::system::error_code &ec) : ec_(ec) {}
  const boost::system::error_code &error() const { return ec_; }

  operator bool() const { return ec_ ? false : true; }

private:
  boost::system::error_code ec_;
};

;
using result_nullopt_t = result_t<void>;

} // namespace scio

#endif /* scio_C8E90AED_6BBD_46A3_A06D_BA54EB9E7B83 */
