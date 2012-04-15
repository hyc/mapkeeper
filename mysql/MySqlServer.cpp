/*
 * Copyright 2012 Yahoo! Inc.
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
 */

/**
 * This is a implementation of the mapkeeper interface that uses 
 * mysql.
 */
#include <mysql.h>
#include <mysqld_error.h>
#include <arpa/inet.h>
#include <cstdio>
#include "MapKeeper.h"
#include <boost/thread/tss.hpp>
#include "MySqlClient.h"

#include <protocol/TBinaryProtocol.h>
#include <server/TThreadedServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>


using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

using namespace mapkeeper;

class MySqlServer: virtual public MapKeeperIf {
public:
    MySqlServer(const std::string& host, uint32_t port) :
        host_(host),
        port_(port) {
    }

    ResponseCode::type ping() {
        return ResponseCode::Success;
    }

    ResponseCode::type addMap(const std::string& mapName) {
        initMySqlClient();
        return mysql_->createTable(mapName);
    }

    ResponseCode::type dropMap(const std::string& mapName) {
        initMySqlClient();
        return mysql_->dropTable(mapName);
    }

    void listMaps(StringListResponse& _return) {
        initMySqlClient();
        mysql_->listTables(_return.values);
        _return.responseCode = ResponseCode::Success;
    }

    void scan(RecordListResponse& _return, const std::string& mapName, const ScanOrder::type order,
              const std::string& startKey, const bool startKeyIncluded, 
              const std::string& endKey, const bool endKeyIncluded,
              const int32_t maxRecords, const int32_t maxBytes) {
        initMySqlClient();
        mysql_->scan(_return, mapName, order, startKey, startKeyIncluded, endKey, endKeyIncluded, maxRecords, maxBytes);
    }

    void get(BinaryResponse& _return, const std::string& mapName, const std::string& key) {
        initMySqlClient();
        _return.responseCode = mysql_->get(mapName, key, _return.value);
    }

    ResponseCode::type put(const std::string& mapName, const std::string& key, const std::string& value) {
        return ResponseCode::Success;
    }

    ResponseCode::type insert(const std::string& mapName, const std::string& key, const std::string& value) {
        initMySqlClient();
        return mysql_->insert(mapName, key, value);
    }

    ResponseCode::type update(const std::string& mapName, const std::string& key, const std::string& value) {
        initMySqlClient();
        return mysql_->update(mapName, key, value);
    }

    ResponseCode::type remove(const std::string& mapName, const std::string& key) {
        initMySqlClient();
        return mysql_->remove(mapName, key);
    }

private:
    void initMySqlClient() {
        if (mysql_.get() == NULL) {
            mysql_.reset(new MySqlClient(host_, port_));
        }
    }

    boost::thread_specific_ptr<MySqlClient> mysql_;
    std::string host_;
    uint32_t port_;
};

int main(int argc, char **argv) {
    int port = 9090;
    shared_ptr<MySqlServer> handler(new MySqlServer("localhost", 3306));
    shared_ptr<TProcessor> processor(new MapKeeperProcessor(handler));
    shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
    TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();
    return 0;
}
