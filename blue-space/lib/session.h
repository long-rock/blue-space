#pragma once

#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>

#include <memory>
#include <string_view>

namespace beast = boost::beast;

namespace application
{
class Session : public std::enable_shared_from_this<Session>
{
  public:
    using Ptr = std::shared_ptr<Session>;

    using RequestHandler = std::function<void(std::string_view view, Ptr session)>;

    using Socket = boost::asio::ip::tcp::socket;
    using Body = beast::http::string_body;
    using RequestParser = beast::http::request_parser<Body>;

    Session(boost::asio::io_context &ioc);

    Socket &socket() {
        return stream_.socket();
    }

    void set_request_handler(RequestHandler &&handler);

    void start();
    void stop();

    template<class B>
    void handle_request(beast::http::request<B> &&request);

    void send_response(std::string_view response);

    template<class Message>
    void send_message(Message &&message);
  private:
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    beast::tcp_stream stream_;

    beast::flat_buffer buffer_;
    std::unique_ptr<RequestParser> parser_;
    std::unique_ptr<RequestHandler> on_request_;
};
} // namespace application