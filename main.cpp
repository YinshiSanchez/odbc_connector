#include <cassert>
#include "connection_pool.hpp"
#include "odbc_connector.hpp"

int main() {
  // ODBCConnectorSingletone &dbc = ODBCConnectorSingletone::GetInstance();

  // // MySQLDSN 是配置里写好的一套配置
  // if (dbc.Connect("MPPODBC", "yinshi", "gauss@123")) {
  //   std::cout << "Connected to database successfully!" << std::endl;

  //   if (dbc.ExecuteQuery("SELECT * FROM employees;")) {
  //     // Fetch and process results
  //     dbc.FetchResults();
  //   } else {
  //     std::cerr << "Query failed: " << dbc.GetErrorMessage() << std::endl;
  //   }

  //   dbc.Disconnect();
  // } else {
  //   std::cerr << "Connection failed: " << dbc.GetErrorMessage() << std::endl;
  // }
  ConnectionPool::Init(1, "MPPODBC", "yinshi", "zzx1230.");
  ConnectionPool &pool = ConnectionPool::GetInstance();
  assert(pool.AvailableConnection() == 1);
  {
    auto guard = pool.GetConnector();
    auto &connector = guard.Get();

    SQLRequest req;
    req.SetSQL("select * from orders;");
    connector.ExecuteQuery(req);

    req.FetchResults();
    assert(pool.AvailableConnection() == 0);
  }
  assert(pool.AvailableConnection() == 1);
  {
    auto guard = pool.GetConnector();
    auto &connector = guard.Get();

    SQLRequest req;
    req.SetSQL("select * from users;");
    connector.ExecuteQuery(req);

    req.FetchResults();
    assert(pool.AvailableConnection() == 0);
  }
  assert(pool.AvailableConnection() == 1);

  return 0;
}
