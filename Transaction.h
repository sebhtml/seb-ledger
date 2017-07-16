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

#ifndef Transaction_H
#define Transaction_H

#include <string>

class Transaction
{
public:
    Transaction(const std::string & accountName, const std::string & currency, const double amount);
    ~Transaction();
    const std::string & getAccountName() const;
    const std::string & getCurrency() const;
    double getAmount() const;

private:

    std::string m_accountName;
    std::string m_currency;
    double m_amount;
};

#endif // Transaction_H

