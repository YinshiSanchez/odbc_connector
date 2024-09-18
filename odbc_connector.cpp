#include "odbc_connector.hpp"
#include <iostream>

ODBCConnector &ODBCConnector::GetInstance() {
  static ODBCConnector singletone;
  return singletone;
}

ODBCConnector::ODBCConnector()
    : henv_(nullptr), hdbc_(nullptr), hstmt_(nullptr), ret_(SQL_SUCCESS) {
  // Allocate an environment handle
  ret_ = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv_);
  if (ret_ != SQL_SUCCESS) {
    last_error_ = "Error allocating environment handle";
  }

  SQLSetEnvAttr(henv_, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
}

ODBCConnector::~ODBCConnector() {
  Disconnect();
  if (henv_) {
    SQLFreeHandle(SQL_HANDLE_ENV, henv_);
  }
}

bool ODBCConnector::Connect(const std::string &dsn, const std::string &user,
                            const std::string &password) {
  // Allocate connection handle
  ret_ = SQLAllocHandle(SQL_HANDLE_DBC, henv_, &hdbc_);
  if (ret_ != SQL_SUCCESS) {
    HandleError(SQL_HANDLE_ENV, henv_);
    return false;
  }

  // Connect to the database
  ret_ = SQLConnect(hdbc_, (SQLCHAR *)dsn.c_str(), SQL_NTS,
                    (SQLCHAR *)user.c_str(), SQL_NTS,
                    (SQLCHAR *)password.c_str(), SQL_NTS);
  if (ret_ != SQL_SUCCESS && ret_ != SQL_SUCCESS_WITH_INFO) {
    HandleError(SQL_HANDLE_DBC, hdbc_);
    return false;
  }

  return true;
}

void ODBCConnector::Disconnect() {
  if (hstmt_) {
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
    hstmt_ = nullptr;
  }

  if (hdbc_) {
    SQLDisconnect(hdbc_);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
    hdbc_ = nullptr;
  }
}

bool ODBCConnector::ExecuteQuery(const std::string &query) {
  if (!hdbc_) {
    last_error_ = "Not connected to a database.";
    return false;
  }

  // Allocate statement handle
  if (!hstmt_) {
    ret_ = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);
    if (ret_ != SQL_SUCCESS) {
      HandleError(SQL_HANDLE_DBC, hdbc_);
      return false;
    }
  }

  // Execute SQL query
  ret_ = SQLExecDirect(hstmt_, (SQLCHAR *)query.c_str(), SQL_NTS);
  if (ret_ != SQL_SUCCESS && ret_ != SQL_SUCCESS_WITH_INFO) {
    HandleError(SQL_HANDLE_STMT, hstmt_);
    return false;
  }

  return true;
}

bool ODBCConnector::FetchResults() {
  if (!hstmt_) {
    last_error_ = "No active statement handle.";
    return false;
  }

  char buffer[256];
  // SQLFetch每次拿一行，SQLGetData拿第一列
  // 我看文档里拿数据好像还有别的做法。
  while (SQLFetch(hstmt_) == SQL_SUCCESS) {
    SQLGetData(hstmt_, 1, SQL_C_CHAR, buffer, sizeof(buffer), nullptr);
    std::cout << "Result: " << buffer << std::endl;
  }

  return true;
}

std::string ODBCConnector::GetErrorMessage() const { return last_error_; }

void ODBCConnector::CleanUp() {
  if (hstmt_) {
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
    hstmt_ = nullptr;
  }
}

void ODBCConnector::HandleError(SQLSMALLINT handleType, SQLHANDLE handle) {
  SQLCHAR sqlState[6], message[256];
  SQLINTEGER nativeError;
  SQLSMALLINT messageLength;
  last_error_.clear();

  if (SQLGetDiagRec(handleType, handle, 1, sqlState, &nativeError, message,
                    sizeof(message), &messageLength) == SQL_SUCCESS) {
    last_error_ = std::string((char *)message);
  } else {
    last_error_ = "Unknown error occurred.";
  }

  std::cerr << "ODBC Error: " << last_error_ << std::endl;
}
