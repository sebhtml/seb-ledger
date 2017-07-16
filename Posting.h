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

#ifndef Posting_H
#define Posting_H

#include "Transaction.h"

#include <map>
#include <fstream>
#include <vector>

class Posting
{
public:

    Posting(size_t id, const std::string & date, const std::string & description);
    ~Posting();

    void addTransactionLine(const std::string & transactionLine);

    bool operator<(const Posting & b) const;

private:

    size_t m_id;
    std::string m_date;
    std::string m_description;
    std::vector<std::string> m_transactionLines;
    std::vector<Transaction> m_transactions;

};

#endif // Posting_H
