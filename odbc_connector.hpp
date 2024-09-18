#ifndef ODBC_CONNECTOR_H
#define ODBC_CONNECTOR_H

#include <sql.h>
#include <sqlext.h>
#include <string>

// 我写了个简单的单例模式，避免资源浪费，跑不是特别大的benchmark应该是够用的
// 如果并发度特别高，也可以写个connector的池子。
class ODBCConnector {
public:

  //singletone
  static ODBCConnector& GetInstance();

  ODBCConnector(const ODBCConnector&) = delete;
  ODBCConnector& operator=(const ODBCConnector&) = delete;

  bool Connect(const std::string &dsn, const std::string &user,
               const std::string &password);
  void Disconnect();
  bool ExecuteQuery(const std::string &query);
  bool FetchResults();
  std::string GetErrorMessage() const;

private:
  SQLHENV henv_;
  SQLHDBC hdbc_;
  SQLHSTMT hstmt_;
  SQLRETURN ret_;
  std::string last_error_;

  // singletone
  ODBCConnector();
  ~ODBCConnector();

  void CleanUp();
  void HandleError(SQLSMALLINT handleType, SQLHANDLE handle);
};

#endif // ODBC_CONNECTOR_H
