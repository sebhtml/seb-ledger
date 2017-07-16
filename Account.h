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

#ifndef Account_H
#define Account_H

#include <string>
#include <vector>
#include "Transaction.h"

class Account
{
public:
    Account(const std::string & name, const std::string & currency);
    ~Account();
    const std::string & getName() const;
    const std::string & getCurrency() const;
    double getBalance() const;
    void addTransaction(const Transaction & transaction);
    const std::vector<const Transaction *> & getTransactions() const;

private:

    std::string m_name;
    std::string m_currency;
    double m_balance;
   
    // TODO: use std::list
    std::vector<const Transaction *> m_transactions;
};

#endif // Account_H

