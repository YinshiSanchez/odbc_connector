#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include "connector_guard.hpp"
#include "odbc_connector.hpp"

#include <sqltypes.h>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <vector>

class ConnectionPool {
 public:
  static ConnectionPool &GetInstance() { return singleton; }

  static void Init(const uint32_t max_connections, const std::string &dsn, const std::string &user,
                   const std::string &passwd) {
    std::unique_lock lock(singleton.queue_mtx_);

    singleton.connectors_ = std::vector<ODBCConnector>(max_connections);
    singleton.max_connections_ = max_connections;

    for (int i = 0; i < max_connections; ++i) {
      singleton.connectors_[i].Connnect(singleton.henv_, dsn, user, passwd);
      singleton.free_list_.push(i);
    }
  }

  ConnectionPool(const ConnectionPool &) = delete;

  ConnectionPool &operator=(const ConnectionPool &) = delete;

  ConnectorGuard GetConnector();

  void FreeConnector(uint idx);

  uint32_t MaxConnections() const { return max_connections_; }

  uint32_t AvailableConnection() const { return free_list_.size(); }

 private:
  static ConnectionPool singleton;

  uint32_t max_connections_;
  SQLHENV henv_;

  std::mutex queue_mtx_;
  std::condition_variable cv_;

  std::vector<ODBCConnector> connectors_;
  std::queue<uint> free_list_;

  ConnectionPool();

  ~ConnectionPool();
};

#endif