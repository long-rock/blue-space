#include "application/rpc/server.h"

#include <jsonrpc-lean/server.h>
#include <jsonrpc-lean/value.h>

#include <boost/log/trivial.hpp>
#include <boost/core/ignore_unused.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <string_view>
#include <vector>

using namespace application::rpc;

Server::Server() : rpc_server_(), json_format_handler_()
{
}

void Server::initialize()
{
    rpc_server_.RegisterFormatHandler(json_format_handler_);
}

void Server::register_method(const std::string &name, jsonrpc::MethodWrapper::Method method)
{
    auto &dispatcher = rpc_server_.GetDispatcher();
    dispatcher.AddMethod(name, std::move(method));
}

void Server::handle_request(const std::string &path, std::string_view request, const ResponseHandler &cb)
{
    boost::ignore_unused(path);
    if (request.size() == 0)
    {
        return;
    }

    std::shared_ptr<jsonrpc::FormattedData> output_data = rpc_server_.HandleRequest(std::string(request));
    cb(output_data->GetData());
}