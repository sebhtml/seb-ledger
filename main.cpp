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

#include "LedgerFile.h"
#include <iostream>
#include <algorithm>
#include "Posting.h"
#include <sstream>
#include <ctime>
#include <iomanip>

std::string getTodayDate()
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    std::ostringstream buffer;
    buffer << (now->tm_year + 1900) << '-';
    buffer << std::setfill('0') << std::setw(2) << (now->tm_mon + 1) << '-';
    buffer << std::setfill('0') << std::setw(2) << now->tm_mday;

    return buffer.str();
}

int main(int argc, char ** argv)
{
    if (argc == 1)
    {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " --file master-file.ledger" << std::endl;
        return 0;
    }

    std::string file;
    std::string command;
    std::string selectedAccountName;

    for (int i = 0; i < argc; ++i)
    {
        std::string option = argv[i];

        if (option == "--file" and i + 1 < argc)
        {
            file = argv[i + 1];
        }
        else if (option == "balance")
        {
            command = option;
            if (i + 1 < argc)
            {
                selectedAccountName = argv[i + 1];
            }
        }
    }

    std::vector<Posting> postings;

    if (file != "")
    {
        LedgerFile ledgerFile(file);
        ledgerFile.getPostings(postings);
    }

    std::sort(postings.begin(), postings.end());

    std::map<std::string, Account> accounts;

    //std::cout << "DEBUG postings= " << postings.size() << std::endl;

    const auto today = getTodayDate();

    //std::cout << "DEBUG today= " << today << std::endl;
    
    for (auto & posting : postings)
    {
        if (posting.getDate() > today)
        {
            break;
        }

        posting.compute(accounts);
    }

    if (command == "balance")
    {
        for (const auto & iterator : accounts)
        {
            const auto & account = iterator.second;
            if (selectedAccountName == "" or account.getName() == selectedAccountName)
            {
                const auto balance = account.getBalance();

                if (std::abs(balance) >= MINIMUM_BALANCE)
                {
                    std::cout << account.getName() << "  " << balance << " " << account.getCurrency() << std::endl;
                }
            }
        }
    }
}

