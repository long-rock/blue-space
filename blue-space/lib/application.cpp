#include "application/application.h"

#include "application/rpc/stateless.h"

#include <boost/log/trivial.hpp>

#include <memory>

using namespace application;

Application::Application()
{
    rpc_ = std::make_shared<rpc::Server>();
    server_ = std::make_shared<Server>(rpc_);
}

void Application::initialize(miner::common::Miner::Ptr miner)
{
    rpc_->initialize();
    stateless_ = std::make_shared<api::StatelessApi>(miner);

    rpc::stateless::register_methods(rpc_, stateless_);
}

void Application::start()
{
    BOOST_LOG_TRIVIAL(info) << "Starting application";
    server_->start();
}