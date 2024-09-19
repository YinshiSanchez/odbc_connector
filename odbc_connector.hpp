#ifndef ODBC_CONNECTOR_H
#define ODBC_CONNECTOR_H

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <iostream>
#include <string>

inline void HandleError(SQLSMALLINT handleType, SQLHANDLE handle) {
  SQLCHAR sqlState[6], message[256];
  SQLINTEGER nativeError;
  SQLSMALLINT messageLength;
  std::string err_msg;

  if (SQLGetDiagRec(handleType, handle, 1, sqlState, &nativeError, message, sizeof(message), &messageLength) ==
      SQL_SUCCESS) {
    err_msg = std::string((char *)message);
  } else {
    err_msg = "Unknown error occurred.";
  }

  std::cerr << "ODBC Error: " << err_msg << std::endl;
}

class SQLRequest {
 public:
  SQLRequest();
  ~SQLRequest();

  SQLRequest(const SQLRequest &) = delete;
  SQLRequest &operator=(const SQLRequest &) = delete;

  bool FetchResults();

  void CleanUp();

  void SetSQL(const std::string &str) { sql_str_ = str; }

  void SetSQL(std::string &&str) { sql_str_ = std::move(str); }

  const std::string &SQL() const { return sql_str_; };

  std::string ErrorMsg() const { return err_msg_; }

 private:
  SQLHSTMT hstmt_;
  std::string sql_str_;
  std::string err_msg_;

  friend class ODBCConnector;
};

class ODBCConnector {
 public:
  ODBCConnector();
  ~ODBCConnector();

  bool Connnect(SQLHENV &henv, const std::string &dsn, const std::string &user, const std::string &passwd);

  void Disconnect();

  bool ExecuteQuery(SQLRequest &req);

 private:
  SQLHDBC hdbc_;
};

#endif  // ODBC_CONNECTOR_H
