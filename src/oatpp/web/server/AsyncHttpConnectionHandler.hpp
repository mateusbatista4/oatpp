/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#ifndef oatpp_web_server_AsyncHttpConnectionHandler_hpp
#define oatpp_web_server_AsyncHttpConnectionHandler_hpp

#include "oatpp/web/server/HttpProcessor.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/server/handler/Interceptor.hpp"

#include "oatpp/network/ConnectionHandler.hpp"
#include "oatpp/core/async/Executor.hpp"

namespace oatpp { namespace web { namespace server {

/**
 * Asynchronous &id:oatpp::network::ConnectionHandler; for handling http communication.
 */
class AsyncHttpConnectionHandler : public base::Countable, public network::ConnectionHandler {
private:
  std::shared_ptr<oatpp::async::Executor> m_executor;
private:
  std::shared_ptr<HttpProcessor::Components> m_components;
public:

  /**
   * Constructor.
   * @param components - &id:oatpp::web::server::HttpProcessor::Components;.
   * @param threadCount - number of threads.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpProcessor::Components>& components, v_int32 threadCount = oatpp::async::Executor::VALUE_SUGGESTED);

  /**
   * Constructor.
   * @param components - &id:oatpp::web::server::HttpProcessor::Components;.
   * @param executor - &id:oatpp::async::Executor;.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpProcessor::Components>& components, const std::shared_ptr<oatpp::async::Executor>& executor);

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @param threadCount - number of threads.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router, v_int32 threadCount = oatpp::async::Executor::VALUE_SUGGESTED)
    : AsyncHttpConnectionHandler(std::make_shared<HttpProcessor::Components>(router), threadCount)
  {}

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @param executor - &id:oatpp::async::Executor;.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router, const std::shared_ptr<oatpp::async::Executor>& executor)
    : AsyncHttpConnectionHandler(std::make_shared<HttpProcessor::Components>(router), executor)
  {}

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @param config - &id:oatpp::web::server::HttpProcessor::Config;.
   * @param threadCount - number of threads.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router,
                             const std::shared_ptr<HttpProcessor::Config>& config,
                             v_int32 threadCount = oatpp::async::Executor::VALUE_SUGGESTED)
    : AsyncHttpConnectionHandler(std::make_shared<HttpProcessor::Components>(router, config), threadCount)
  {}

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @param config - &id:oatpp::web::server::HttpProcessor::Config;.
   * @param executor - &id:oatpp::async::Executor;.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router,
                             const std::shared_ptr<HttpProcessor::Config>& config,
                             const std::shared_ptr<oatpp::async::Executor>& executor)
    : AsyncHttpConnectionHandler(std::make_shared<HttpProcessor::Components>(router, config), executor)
  {}

public:
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router,
                                                                  v_int32 threadCount = oatpp::async::Executor::VALUE_SUGGESTED);
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router,
                                                                  const std::shared_ptr<oatpp::async::Executor>& executor);
  
  void setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler);
  
  void addRequestInterceptor(const std::shared_ptr<handler::RequestInterceptor>& interceptor);
  
  void handleConnection(const std::shared_ptr<IOStream>& connection, const std::shared_ptr<const ParameterMap>& params) override;

  /**
   * Will call m_executor.stop()
   */
  void stop() override;
  
};
  
}}}

#endif /* oatpp_web_server_AsyncHttpConnectionHandler_hpp */

