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
#include <sstream>

LedgerFile::LedgerFile(const std::string & file)
: m_file(file)
{
    //std::cout << "DEBUG OPEN file= " << file << std::endl;
}

LedgerFile::~LedgerFile()
{
}


void
LedgerFile::getPostings(std::vector<Posting> & postings)
const
{
    std::ifstream stream(m_file.c_str());
    std::string line;

    std::string path = "";
    size_t result = m_file.find_last_of('/');

    if (result != std::string::npos)
    {
        path = m_file.substr(0, result + 1);
    }

    //std::cout << "DEBUG path= " << path << std::endl;

    size_t numberOfInitialPostings = postings.size();

    while (not stream.eof())
    {
        std::getline(stream, line);
        //std::cout << "DEBUG file= " << m_file << " line= " << line << std::endl;

        std::istringstream lineStream(line);
        std::string token;
        lineStream >> token;

        //std::cout << "DEBUG token= " << token << std::endl;

        if (token == "!include")
        {
            std::string fileName;
            lineStream >> fileName;

            LedgerFile ledgerFile(path + fileName);
            ledgerFile.getPostings(postings);
        }
        else if (token != "")
        {
            std::string date = token;
            std::string description = lineStream.str().substr(date.length());
            
            //std::cout << "DEBUG Posting  date= " << date << "  description= " << description << std::endl;

            Posting posting(date, description);

            // Load transaction lines.

            while (1)
            {
                std::string transactionLine;
                std::getline(stream, transactionLine);
        
                bool isTransactionLine(false);
                if (transactionLine.length() >= 1)
                {
                    if (transactionLine[0] == ' ' or transactionLine[0] == '\t')
                    {
                        std::string account;
                        std::istringstream transactionLineStream(transactionLine);
                        transactionLineStream >> account;
                        if (account != "")
                        {
                            isTransactionLine = true;
                        }
                    }
                }
                if (isTransactionLine)
                {
                    posting.addTransactionLine(transactionLine);
                }
                else
                {
                    break;
                }
            }

            postings.push_back(posting);
        }
    }

    size_t loadedPostings = postings.size() - numberOfInitialPostings;

    std::cout << "DEBUG loaded " << loadedPostings << " postings from file " << m_file << std::endl;
}

