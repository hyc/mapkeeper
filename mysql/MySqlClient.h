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
#ifndef MYSQLCLIENT_H
#define MYSQLCLIENT_H

#include <string>
#include <vector>
#include <mysql.h>
#include "MapKeeper.h"
#include "mapkeeper_types.h"

using namespace std;
using namespace mapkeeper;

class MySqlClient {
public:
    MySqlClient(const std::string& host, uint32_t port);
    ~MySqlClient();
    ResponseCode::type createTable(const std::string& tableName);
    ResponseCode::type dropTable(const std::string& tableName);
    void listTables(vector<string>& tables);
    ResponseCode::type insert(const std::string& tableName, const std::string& key, const std::string& value);
    ResponseCode::type update(const std::string& tableName, const std::string& key, const std::string& value);
    ResponseCode::type get(const std::string& tableName, const std::string& key, std::string& value);
    ResponseCode::type remove(const std::string& tableName, const std::string& key);
    void scan (mapkeeper::RecordListResponse& _return, const std::string& mapName, const mapkeeper::ScanOrder::type order,
            const std::string& startKey, const bool startKeyIncluded,
            const std::string& endKey, const bool endKeyIncluded,
            const int32_t maxRecords, const int32_t maxBytes);

private:
    std::string escapeString(const std::string& str);
    MYSQL mysql_;
    std::string host_;
    uint32_t port_;
};

#endif
