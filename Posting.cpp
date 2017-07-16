/*
    seb-ledger: a ledger program that allows querying any account balance inside a transaction
    Copyright (C) 2017  Sébastien Boisvert

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include "Posting.h"
#include <map>
#include <iostream>
#include <cmath>
#include <sstream>
#include <set>

Posting::Posting(
    size_t id, const std::string & date, const std::string & description)
    : m_id(id)
    , m_date(date)
    , m_description(description)
{
}

Posting::~Posting()
{
}


void Posting::addTransactionLine(const std::string & transactionLine)
{
    m_transactionLines.push_back(transactionLine);
}


bool Posting::operator<(const Posting & b) const
{
    if (m_date == b.m_date)
    {
        return m_id < b.m_id;
    }

    return m_date < b.m_date;
}

void Posting::generateTransactions(
    const std::map<std::string, Account> & accounts
)
{
    std::cout << "DEBUG generateTransactions  m_transactionLines= " << m_transactionLines.size() << std::endl;

    for (const auto & transactionLine : m_transactionLines)
    {
        parseTransaction(transactionLine, accounts);
    }
}

void Posting::parseTransaction(
    const std::string & transactionLine,
    const std::map<std::string, Account> & accounts
)
{
    std::cout << "DEBUG parseTransaction  transactionLine= " << transactionLine << std::endl;

    std::string accountName;

    std::istringstream stream(transactionLine);

    stream >> accountName;

    std::string currency;
    while (1)
    {
        std::string token;
        stream >> token;

        if (token == "")
        {
            break;
        }

        if (token[token.length() - 1] == ')')
        {
            // CAD)
            currency = token.substr(0, token.length() - 1);
        }
        else
        {
            // CAD
            currency = token;
        }
    }

    size_t startOffset = transactionLine.find(accountName) + accountName.length();
    std::string amountString =
        transactionLine.substr(startOffset, transactionLine.length() - startOffset - currency.length());

    // Trim
    while (amountString.length() >= 1 and amountString[0] == ' ' or amountString[1] == '\t')
    {
        amountString = amountString.substr(1);
    }
    while (amountString.length() >= 1 and amountString[amountString.length() - 1] == ' ' or amountString[amountString.length() - 1] == '\t')
    {
        amountString = amountString.substr(0, amountString.length() - 1);
    }

    std::cout << "DEBUG parseTransaction transactionLine= " << transactionLine;
    std::cout << "  accountName= " << accountName;
    std::cout << "  currency= " << currency;
    std::cout << "  amountString= " << amountString;
    std::cout << std::endl;

    bool isSimpleAmount(true);
    std::set<char> simpleAmountCharacters;
    simpleAmountCharacters.insert('-');
    simpleAmountCharacters.insert('0');
    simpleAmountCharacters.insert('1');
    simpleAmountCharacters.insert('2');
    simpleAmountCharacters.insert('3');
    simpleAmountCharacters.insert('4');
    simpleAmountCharacters.insert('5');
    simpleAmountCharacters.insert('6');
    simpleAmountCharacters.insert('7');
    simpleAmountCharacters.insert('8');
    simpleAmountCharacters.insert('9');
    simpleAmountCharacters.insert('.');

    for (size_t i = 0; i < amountString.length(); ++i)
    {
        char character = amountString[i];
        if (simpleAmountCharacters.count(character) == 0)
        {
            isSimpleAmount = false;
            break;
        }
    }

    bool isAutoLine = false;

    if (amountString.length() == 0)
    {
        isAutoLine = true;
        isSimpleAmount = false;
    }

    double amount;

    if (isSimpleAmount)
    {
        std::istringstream amountStream(amountString);
        amountStream >> amount;
    }
    else if (isAutoLine)
    {
        std::cout << "DEBUG auto-line!" << std::endl;

        std::map<std::string, double> balances;
        for (const auto & transaction : m_transactions)
        {
            const auto & accountName2 = transaction.getAccountName();
            const auto & currency2 = transaction.getCurrency();
            const auto amount2 = transaction.getAmount();
            balances[currency2] += amount2;
        }

        bool foundUnbalancedCurrency(false);

        for (const auto & iterator : balances)
        {
            const auto & currency2 = iterator.first;
            const auto & balance2 = iterator.second;

            if (std::abs(balance2) > 0.01)
            {
                if (foundUnbalancedCurrency)
                {
                    std::cerr << "Error: posting can not contain 2 unbalanced currencies." << std::endl;
                }

                amount = -balance2;
                currency = currency2;
                std::cout << "DEBUG auto-balancing, " << accountName << "  " << amount << " " << currency << std::endl;

                foundUnbalancedCurrency = true;
            }
        }
    }
    else
    {
        std::cerr << "Error: unsupported amountString: " << amountString << std::endl;
        exit(1);
    }

    Transaction transaction(accountName, currency, amount);
    m_transactions.push_back(transaction);
}

void Posting::compute(std::map<std::string, Account> & accounts)
{
    generateTransactions(accounts);

    std::cout << "DEBUG Posting::compute m_transactions = " << m_transactions.size() << std::endl;

    if (not (m_transactions.size() >= 2))
    {
        std::cerr << "Error: posting " << getDate() << " " << getDescription() << " has only ";
        std::cerr << m_transactions.size() << " transactions" << std::endl;
        exit(1);
    }

    // Check that posting is balanced.
    std::map<std::string, double> balances;
    for (const auto & transaction : m_transactions)
    {
        const auto & accountName = transaction.getAccountName();
        const auto & currency = transaction.getCurrency();
        const auto amount = transaction.getAmount();
        balances[currency] += amount;
    }

    for (const auto & iterator : balances)
    {
        const auto & currency = iterator.first;
        const auto & balance = iterator.second;

        if (std::abs(balance) > 0.01)
        {
            std::cerr << "Error: posting " << getDate() << " " << getDescription() << " is not balanced." << std::endl;
            exit(1);
        }
    }

    // Apply transactions
    for (const auto & transaction : m_transactions)
    {
        const auto & accountName = transaction.getAccountName();
        const auto & currency = transaction.getCurrency();
        const auto amount = transaction.getAmount();

        if (accounts.count(accountName) == 0)
        {
            Account account(accountName, currency);
            accounts.emplace(accountName, account);
        }

        auto & account = accounts.at(accountName);

        if (account.getCurrency() != currency)
        {
            std::cerr << "Error: account " << accountName << " uses currency " << account.getCurrency();
            std::cerr << " but transaction uses currency " << currency << std::endl;
            exit(1);
        }

        account.applyAmount(amount);
    }
}


const std::string & Posting::getDate() const
{
    return m_date;
}

const std::string & Posting::getDescription() const
{
    return m_description;
}
