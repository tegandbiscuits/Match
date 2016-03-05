#include "catch.hpp"
#include "../src/account.h"
#include <QUrl>
#include <QDate>
#include "../src/json/json.h"
#include "../src/accountmanager.h"

TEST_CASE("Can add checking accounts", "[addChecking]") {
    SECTION("File path, account name, balance, and balance date are given") {
        Account account;
        AccountManager accManager;
        QUrl filePath = QUrl::fromLocalFile("Foo Budget.mbgt");
        QDate balanceDate = QDate(2016, 2, 29);

        account.addChecking(filePath, "Foo CU", 80000, balanceDate);
        Json::Value budget = accManager.loadFile(filePath);
        REQUIRE(budget["onBudgetAccounts"][0]["accountName"] == "Foo CU");
        REQUIRE(budget["onBudgetAccounts"][0]["balance"] == 80000);
        REQUIRE(budget["onBudgetAccounts"][0]["transactions"][0]["note"] == "Initial Balance");
        REQUIRE(budget["balance"] == 80000);
    }
}

TEST_CASE("Can add transactions to account", "[addTransaction]") {
    SECTION("Given file path, account index, date, payee, if outflow, amount, and note") {
        Account account;
        AccountManager accManager;
        QUrl filePath = QUrl::fromLocalFile("Foo Budget.mbgt");
        QDate transactionDate = QDate(2016, 2, 29);

        account.addTransaction(filePath, 0, transactionDate, "Caffe Nero", true, 125, "Espresso");
        Json::Value budget = accManager.loadFile(filePath);

        REQUIRE(budget["onBudgetAccounts"][0]["transactions"][1]["date"] == "2016-02-29");
        REQUIRE(budget["onBudgetAccounts"][0]["transactions"][1]["payee"] == "Caffe Nero");
        REQUIRE(budget["onBudgetAccounts"][0]["transactions"][1]["outflow"] == true);
        REQUIRE(budget["onBudgetAccounts"][0]["transactions"][1]["amount"] == 125);
        REQUIRE(budget["onBudgetAccounts"][0]["transactions"][1]["note"] == "Espresso");
        REQUIRE(budget["onBudgetAccounts"][0]["balance"] == 79875);
        REQUIRE(budget["balance"] == 79875);
    }

    SECTION("If outflow is false, then it's income and should be added") {
        Account account;
        AccountManager accManager;
        QUrl filePath = QUrl::fromLocalFile("Foo Budget.mbgt");
        QDate transactionDate = QDate(2016, 2, 29);

        account.addTransaction(filePath, 0, transactionDate, "Tip", false, 1000, "");
        Json::Value budget = accManager.loadFile(filePath);
        REQUIRE(budget["onBudgetAccounts"][0]["balance"] == 80875);
        REQUIRE(budget["balance"] == 80875);
    }
}

TEST_CASE("Can get a list of accounts and balances", "[getAccountList]") {
    SECTION("Give file path, returns account, index, and balance") {
        Account account;
        QUrl filePath = QUrl::fromLocalFile("Foo Budget.mbgt");

        Json::Value accountList = account.getAccountList(filePath);
        REQUIRE(accountList["balance"] == 80875);
        REQUIRE(accountList["accounts"][0]["accountName"] == "Foo CU");
        REQUIRE(accountList["accounts"][0]["accountBalance"] == 80875);
        REQUIRE(accountList["accounts"][0]["index"] == 0);
    }

    SECTION("Method can return as a QString") {
        Account account;
        QUrl filePath = QUrl::fromLocalFile("Foo Budget.mbgt");

        QString accountString = account.getAccountListString(filePath);
        Json::Value accountList;
        Json::Reader reader;
        reader.parse(accountString.toStdString(), accountList);

        REQUIRE(accountList["balance"] == 80875);
    }
}
