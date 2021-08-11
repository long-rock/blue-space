#include "session.h"

#include <boost/log/trivial.hpp>

#include <chrono>
#include <string_view>
#include <tuple>

using namespace application;

namespace beast = boost::beast;

static const char *SERVER_NAME = "blue-space";

Session::Session(boost::asio::io_context &ioc)
    : strand_(boost::asio::make_strand(ioc)), stream_(boost::asio::ip::tcp::socket(strand_))
{
}

void Session::set_request_handler(RequestHandler &&handler)
{
    on_request_ = std::make_unique<RequestHandler>(std::move(handler));
}

void Session::start()
{
    parser_ = std::make_unique<RequestParser>();
    stream_.expires_after(std::chrono::seconds(30));

    beast::http::async_read(stream_, buffer_, parser_->get(),
                            [self = shared_from_this()](auto ec, auto bytes_transferred) {
                                boost::ignore_unused(bytes_transferred);
                                if (ec)
                                {
                                    if (ec != beast::http::error::end_of_stream)
                                    {
                                        BOOST_LOG_TRIVIAL(error) << "unknown error in session";
                                    }
                                    self->stop();
                                }

                                self->handle_request(self->parser_->release());
                            });
}

void Session::stop()
{
    beast::error_code ec;
    stream_.socket().shutdown(Socket::shutdown_both, ec);
    boost::ignore_unused(ec);
}

template <class B> void Session::handle_request(beast::http::request<B> &&request)
{
    if (on_request_)
    {
        auto body = request.body();
        (*on_request_)(body, shared_from_this());
    }
}

void Session::send_response(std::string_view response)
{
    beast::http::string_body::value_type body;
    body.assign(response);

    const auto size = body.size();

    beast::http::response<beast::http::string_body> res(std::piecewise_construct, std::make_tuple(std::move(body)));
    res.set(beast::http::field::server, SERVER_NAME);
    res.set(beast::http::field::content_type, "application/json");
    res.content_length(size);
    res.keep_alive(true);
    res.prepare_payload();
    return send_message(std::move(res));
}

template <class Message> void Session::send_message(Message &&message)
{
    using message_type = decltype(message);
    auto m = std::make_shared<std::decay_t<message_type>>(std::forward<message_type>(message));

    beast::http::async_write(stream_, *m, [self = shared_from_this(), m](auto ec, auto bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec)
        {
            BOOST_LOG_TRIVIAL(error) << "error writing response: " << ec.message();
            return self->stop();
        }

        if (m->need_eof())
        {
            return self->stop();
        }

        self->start();
    });
}