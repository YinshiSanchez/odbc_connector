#include "connection_pool.hpp"

#include <exception>
#include <iostream>
#include <mutex>
#include "connector_guard.hpp"

ConnectionPool ConnectionPool::singleton;

ConnectorGuard ConnectionPool::GetConnector() {
  std::unique_lock lock(queue_mtx_);
  cv_.wait(lock, [this]() -> bool { return !free_list_.empty(); });

  uint connector_idx = free_list_.front();
  free_list_.pop();

  return ConnectorGuard{&connectors_[connector_idx]};
}

void ConnectionPool::FreeConnector(uint idx) {
  std::unique_lock lock(queue_mtx_);
  free_list_.push(idx);
}

ConnectionPool::ConnectionPool() {
  if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv_) != SQL_SUCCESS) {
    std::cerr << "Error allocating environment handle\n";
    std::terminate();
  }

  SQLSetEnvAttr(henv_, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
}

ConnectionPool::~ConnectionPool() {
  for (int i = 0; i < max_connections_; ++i) {
    connectors_[i].Disconnect();
  }

  if (henv_) {
    SQLFreeHandle(SQL_HANDLE_ENV, henv_);
  }
}
