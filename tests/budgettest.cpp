#include "catch.hpp"
#include "../src/budget.h"
#include "../src/sqlite/sqlite.hpp"
#include <QUrl>
#include <QDate>
#include <iostream>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

// Budget account and stuff made in matchtest file
// Separated to make sure there's a control

QUrl budgetFilePath = QUrl::fromLocalFile("BudgetTestAccount.mbgt");
Budget budget;

QString currentMonth = QDate::currentDate().toString("yyyy-MM");
QString monthPlusOne = QDate::currentDate().addMonths(1).toString("yyyy-MM");
QString monthPlusTwo = QDate::currentDate().addMonths(2).toString("yyyy-MM");
QString monthMinusOne = QDate::currentDate().addMonths(-1).toString("yyyy-MM");
QString monthMinusTwo = QDate::currentDate().addMonths(-2).toString("yyyy-MM");

TEST_CASE("Can add budget categories", "[addCategory]") {
    SECTION("Give file path, name, and initial amount") {
        budget.addCategory(budgetFilePath, "Test Budget", 10000);

        io::sqlite::db mbgt("BudgetTestAccount.mbgt");
        io::sqlite::stmt query(mbgt, "SELECT categoryName, monthOne,"
                                     "monthOneSpent, monthOneDate,"
                                     "monthTwo, monthTwoSpent, monthTwoDate,"
                                     "monthThree, monthThreeSpent, monthThreeDate,"
                                     "prevOne, prevOneSpent, prevOneDate,"
                                     "prevTwo, prevTwoSpent, prevTwoDate FROM budgets");

        while (query.step()) {
            std::cout << "1: addCategory(1)\n";
            // categoryName
            REQUIRE(query.row().text(0) == "Test Budget");
            // monthOne
            REQUIRE(query.row().int32(1) == 10000);
            // monthOneSpent
            REQUIRE(query.row().int32(2) == 0);
            // monthOneDate
            REQUIRE(query.row().text(3) == currentMonth.toStdString());
            // monthTwo
            REQUIRE(query.row().int32(4) == 0);
            // monthTwoSpent
            REQUIRE(query.row().int32(5) == 0);
            // monthTwoDate
            REQUIRE(query.row().text(6) == monthPlusOne.toStdString());
            // monthThree
            REQUIRE(query.row().int32(7) == 0);
            // monthThreeSpent
            REQUIRE(query.row().int32(8) == 0);
            // monthThreeDate
            REQUIRE(query.row().text(9) == monthPlusTwo.toStdString());
            // prevOne
            REQUIRE(query.row().int32(10) == 0);
            // prevOneSpent
            REQUIRE(query.row().int32(11) == 0);
            // prevOneDate
            REQUIRE(query.row().text(12) == monthMinusOne.toStdString());
            // prevTwo
            REQUIRE(query.row().int32(13) == 0);
            // prevTwoSpent
            REQUIRE(query.row().int32(14) == 0);
            // prevTwoDate
            REQUIRE(query.row().text(15) == monthMinusTwo.toStdString());
        }
    }
}

TEST_CASE("Can get a list of categories with amounts for a month at relative index", "[getCategories]") {
    SECTION("Zero is current month") {
        QJsonArray categories = budget.getCategories(budgetFilePath, 0);

        REQUIRE(categories[0].toObject()["categoryName"] == "Test Budget");
        REQUIRE(categories[0].toObject()["amount"] == "100.00");
        REQUIRE(categories[0].toObject()["remaining"] == "100.00");
    }

    SECTION("Can go positive or negative up to 2 out") {
        QJsonArray categories = budget.getCategories(budgetFilePath, 1);
        REQUIRE(categories[0].toObject()["categoryName"] == "Test Budget");
        REQUIRE(categories[0].toObject()["amount"] == "0.00");

        categories = budget.getCategories(budgetFilePath, 2);
        REQUIRE(categories[0].toObject()["categoryName"] == "Test Budget");

        categories = budget.getCategories(budgetFilePath, -1);
        REQUIRE(categories[0].toObject()["categoryName"] == "Test Budget");

        categories = budget.getCategories(budgetFilePath, -2);
        REQUIRE(categories[0].toObject()["categoryName"] == "Test Budget");
    }

    SECTION("Out of that range, has error") {
        QJsonArray categories = budget.getCategories(budgetFilePath, 3);
        REQUIRE(categories[0].toObject()["err"] == "Out of stored range");

        categories = budget.getCategories(budgetFilePath, -3);
        REQUIRE(categories[0].toObject()["err"] == "Out of stored range");
    }
}

TEST_CASE("Can get a list of only category names", "[getCategoryNames]") {
    SECTION("Returns an QList of the category names") {
        QList<QString> categories = budget.getCategoryNames(budgetFilePath);

        REQUIRE(categories.at(0) == "Test Budget");
    }

    SECTION("The list is alphabetized") {
        budget.addCategory(budgetFilePath, "Hello World", 20000);
        QList<QString> categories = budget.getCategoryNames(budgetFilePath);

        REQUIRE(categories.at(0) == "Hello World");
        REQUIRE(categories.at(1) == "Test Budget");
    }
}

TEST_CASE("Can subtract from remaining amount", "[addToSpent]") {
    SECTION("Give file path, category, month date, and amount; returns bool") {
        bool changeSuccess = budget.addToSpent(budgetFilePath,
                                                       "Test Budget",
                                                       currentMonth, 5000);

        REQUIRE(changeSuccess == true);

        io::sqlite::db mbgt("BudgetTestAccount.mbgt");
        io::sqlite::stmt query(mbgt, "SELECT monthOneSpent FROM budgets WHERE id == 1");

        while (query.step()) {
            std::cout << "2: addToSpent(1)\n";
            REQUIRE(query.row().int32(0) == 5000);
        }
    }

    SECTION("It works with past and future months") {
        budget.addToSpent(budgetFilePath,
                                  "Test Budget",
                                  monthPlusOne, 5000);
        budget.addToSpent(budgetFilePath,
                                  "Test Budget",
                                  monthPlusTwo, 5000);
        budget.addToSpent(budgetFilePath,
                                  "Test Budget",
                                  monthMinusOne, 5000);
        budget.addToSpent(budgetFilePath,
                                  "Test Budget",
                                  monthMinusTwo, 5000);

        io::sqlite::db mbgt("BudgetTestAccount.mbgt");
        io::sqlite::stmt query(mbgt, "SELECT monthTwoSpent, monthThreeSpent,"
                                     "prevOneSpent, prevTwoSpent"
                                     " FROM budgets WHERE id == 1");

        while (query.step()) {
            std::cout << "2: addToSpent(2)\n";
            REQUIRE(query.row().int32(0) == 5000);
            REQUIRE(query.row().int32(1) == 5000);
            REQUIRE(query.row().int32(2) == 5000);
            REQUIRE(query.row().int32(3) == 5000);
        }
    }

    SECTION ("If the month is out of range, it returns false") {
        QString outOfDate = QDate::currentDate().addMonths(4).toString("yyyy-MM");
        bool changeStatus = budget.addToSpent(budgetFilePath,
                                                      "Test Budget",
                                                      outOfDate, 5000);
        REQUIRE(changeStatus == false);
    }
}

TEST_CASE("Can update budget for month", "[updateBudget]") {
    SECTION("File path, month number, categoryName, and updated amount") {
        budget.updateBudget(budgetFilePath, -2, "Test Budget", 5000);
        budget.updateBudget(budgetFilePath, -1, "Test Budget", 5000);
        budget.updateBudget(budgetFilePath, 0, "Test Budget", 5000);
        budget.updateBudget(budgetFilePath, 1, "Test Budget", 5000);
        budget.updateBudget(budgetFilePath, 2, "Test Budget", 5000);

        io::sqlite::db mbgt("BudgetTestAccount.mbgt");
        io::sqlite::stmt query(mbgt, "SELECT "
                                     "prevTwo, prevOne,"
                                     "monthOne, monthTwo, monthThree "
                                     "FROM budgets WHERE id == 1");

        while (query.step()) {
            std::cout << "3: updateBudget (1)\n";
            REQUIRE(query.row().int32(0) == 5000);
            REQUIRE(query.row().int32(1) == 5000);
            REQUIRE(query.row().int32(2) == 5000);
            REQUIRE(query.row().int32(3) == 5000);
            REQUIRE(query.row().int32(4) == 5000);
        }
    }
}
