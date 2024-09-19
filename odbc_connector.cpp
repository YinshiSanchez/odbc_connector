#include "odbc_connector.hpp"
#include <iostream>

SQLRequest::SQLRequest() : hstmt_(nullptr), sql_str_(), err_msg_() {}

SQLRequest::~SQLRequest() {
  if (hstmt_) {
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
    hstmt_ = nullptr;
  }
}

bool SQLRequest::FetchResults() {
  if (!hstmt_) {
    err_msg_ = "No active statement handle.";
    return false;
  }

  char buffer[256];
  while (SQLFetch(hstmt_) == SQL_SUCCESS) {
    SQLGetData(hstmt_, 1, SQL_C_CHAR, buffer, sizeof(buffer), nullptr);
    std::cout << "Result: " << buffer << std::endl;
  }

  return true;
}

void SQLRequest::CleanUp() {
  if (hstmt_) {
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
    hstmt_ = nullptr;
  }
  sql_str_ = {};
  err_msg_ = {};
}

ODBCConnector::ODBCConnector() : hdbc_(nullptr) {}

ODBCConnector::~ODBCConnector() {
  if (hdbc_) {
    SQLDisconnect(hdbc_);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
    hdbc_ = nullptr;
  }
}

bool ODBCConnector::Connnect(SQLHENV &henv, const std::string &dsn, const std::string &user,
                             const std::string &passwd) {
  if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc_) != SQL_SUCCESS) {
    HandleError(SQL_HANDLE_ENV, henv);
    return false;
  }

  // Connect to the database
  auto ret = SQLConnect(hdbc_, (SQLCHAR *)dsn.c_str(), SQL_NTS, (SQLCHAR *)user.c_str(), SQL_NTS,
                        (SQLCHAR *)passwd.c_str(), SQL_NTS);
  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
    HandleError(SQL_HANDLE_DBC, hdbc_);
    return false;
  }

  return true;
}

void ODBCConnector::Disconnect() {
  if (hdbc_) {
    SQLDisconnect(hdbc_);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
    hdbc_ = nullptr;
  }
}

bool ODBCConnector::ExecuteQuery(SQLRequest &req) {
  if (!hdbc_) {
    std::cerr << "ODBC ERROR: Not connected to a database.\n";
    return false;
  }

  // Allocate statement handle
  if (!req.hstmt_) {
    if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &req.hstmt_) != SQL_SUCCESS) {
      HandleError(SQL_HANDLE_DBC, hdbc_);
      return false;
    }
  }

  // Execute SQL query
  auto ret_ = SQLExecDirect(req.hstmt_, (SQLCHAR *)req.SQL().c_str(), SQL_NTS);
  if (ret_ != SQL_SUCCESS && ret_ != SQL_SUCCESS_WITH_INFO) {
    HandleError(SQL_HANDLE_STMT, req.hstmt_);
    return false;
  }

  return true;
}