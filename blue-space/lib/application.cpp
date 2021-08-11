#include "application/application.h"

#include <boost/log/trivial.hpp>

using namespace application;

Application::Application(RpcServer::Ptr rpc) : rpc_(rpc) {
    server_ = std::make_shared<Server>(rpc);
}

void Application::start()
{
    BOOST_LOG_TRIVIAL(info) << "Starting application";
    server_->start();
}