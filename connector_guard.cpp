#include "connector_guard.hpp"
#include "connection_pool.hpp"

ConnectorGuard::~ConnectorGuard() {
  Drop();
}

ConnectorGuard::ConnectorGuard(ConnectorGuard &&) noexcept {}

ConnectorGuard &ConnectorGuard::operator=(ConnectorGuard &&rhs) noexcept { return *this; }

void ConnectorGuard::Drop() {
  if (connector_) {
    ConnectionPool::GetInstance().FreeConnector(idx_);
    connector_ = nullptr;
  }
}
