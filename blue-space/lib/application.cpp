#include "application/application.h"

#include "application/rpc/stateless.h"

#include <boost/log/trivial.hpp>

#include <memory>

using namespace application;

Application::Application(Application::Options options) : options_(options)
{
    rpc_ = std::make_shared<rpc::Server>();
    rest_ = std::make_shared<rest::Server>();
    server_ = std::make_shared<Server>(rpc_, rest_);
}

void Application::initialize(miner::common::Miner::Ptr miner)
{
    rpc_->initialize();
    stateless_ = std::make_shared<api::StatelessApi>(miner);
    rest_->initialize(stateless_);

    rpc::stateless::register_methods(rpc_, stateless_);
}

void Application::start()
{
    BOOST_LOG_TRIVIAL(info) << "Starting application";
    server_->start(options_.http_port);
}