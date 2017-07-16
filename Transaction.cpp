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


#include "Transaction.h"
#include "Posting.h"

Transaction::Transaction(
    const Posting & posting,
    const std::string & accountName, const std::string & currency, const double amount)
: m_posting(& posting)
, m_accountName(accountName)
, m_currency(currency)
, m_amount(amount)
{
}

Transaction::~Transaction()
{
}

const std::string & Transaction::getDate() const
{
    return m_posting->getDate();
}



const std::string & Transaction::getDescription() const
{
    return m_posting->getDescription();
}

const std::string & Transaction::getAccountName() const
{
    return m_accountName;
}

const std::string & Transaction::getCurrency() const
{
    return m_currency;
}

double Transaction::getAmount() const
{
    return m_amount;
}

