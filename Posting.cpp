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

