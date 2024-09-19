#ifndef CONNECTOR_GUARD_H
#define CONNECTOR_GUARD_H

#include "odbc_connector.hpp"

// make sure to free connection when leave scope
class ConnectorGuard {
 public:
  ConnectorGuard() = default;
  ConnectorGuard(ODBCConnector *ptr) : connector_(ptr) {}
  ~ConnectorGuard();

  ConnectorGuard(const ConnectorGuard &) = delete;
  ConnectorGuard &operator=(const ConnectorGuard &) = delete;

  ConnectorGuard(ConnectorGuard &&) noexcept;
  ConnectorGuard &operator=(ConnectorGuard &&) noexcept;

  void Drop();

  ODBCConnector &Get() { return *connector_; }

 private:
  ODBCConnector *connector_;
  uint idx_;
};

#endif