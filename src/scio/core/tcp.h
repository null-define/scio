#ifndef scio_ED03A4B8_682A_4F8A_86DD_F2B06C2AFA18
#define scio_ED03A4B8_682A_4F8A_86DD_F2B06C2AFA18

#include <boost/asio/ip/tcp.hpp>
#include <scio/core/beast.h>

namespace scio {
namespace tcp {

using endpoint = net::ip::tcp::endpoint;
using resolver = default_token::as_default_on_t<net::ip::tcp::resolver>;
using acceptor = default_token::as_default_on_t<net::ip::tcp::acceptor>;
using socket = default_token::as_default_on_t<net::ip::tcp::socket>;
using stream = default_token::as_default_on_t<beast::tcp_stream>;
} // namespace tcp

} // namespace scio

#endif /* scio_ED03A4B8_682A_4F8A_86DD_F2B06C2AFA18 */
