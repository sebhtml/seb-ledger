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

}

void Posting::compute(std::map<std::string, Account> & accounts)
{
    generateTransactions(accounts);

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
