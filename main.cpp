#include "odbc_connector.hpp"
#include <iostream>

int main() {
    ODBCConnector& dbc = ODBCConnector::GetInstance();

    // MySQLDSN 是配置里写好的一套配置
    if (dbc.Connect("MySQLDSN", "yinshi", "zzx1230.")) {
        std::cout << "Connected to database successfully!" << std::endl;

        if (dbc.ExecuteQuery("SELECT * FROM orders;")) {
            // Fetch and process results
            dbc.FetchResults();
        } else {
            std::cerr << "Query failed: " << dbc.GetErrorMessage() << std::endl;
        }

        dbc.Disconnect();
    } else {
        std::cerr << "Connection failed: " << dbc.GetErrorMessage() << std::endl;
    }

    return 0;
}
