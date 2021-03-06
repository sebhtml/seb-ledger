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
#include <iomanip>

#define MINIMUM_UNBALANCED_AMOUNT 0.01

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
    //std::cout << "DEBUG generateTransactions  date= " << getDate() << "  m_transactionLines= " << m_transactionLines.size() << std::endl;

    for (const auto & transactionLine : m_transactionLines)
    {
        parseTransaction(transactionLine, accounts);
    }
}

double generateResult(double a, double b, const std::string & operation)
{
    if (operation == "*")
    {
        return a * b;
    }
    if (operation == "/")
    {
        return a / b;
    }
    if (operation == "+")
    {
        return a + b;
    }
    if (operation == "-")
    {
        return a - b;
    }
    if (operation == "^")
    {
        return std::pow(a, b);
    }

    std::cerr << "Error: invalid operation: " << operation << std::endl;
    exit(1);

    return 0;
}

bool
simplifyOperation(const std::string & operation,
    std::vector<std::string> & equation, size_t start, size_t end
)
{
    for (int i = start; i <= end; ++i)
    {
        if (equation[i] == operation and i - 1 >= 0 and equation[i - 1] != ")" and equation[i + 1] != "(")
        {
            if (i == 0)
            {
                // Impossible
                continue;
            }

            double left;
            double right;

            std::istringstream leftStream(equation[i - 1]);
            std::istringstream rightStream(equation[i + 1]);
            leftStream >> left;
            rightStream >> right;
            double result = generateResult(left, right, operation);
            std::ostringstream resultStream;
            resultStream << std::setprecision(128) << /**/ result;

            std::vector<std::string> newEquation;
            for (int j = 0; j <= i - 2; ++j)
            {
                newEquation.push_back(equation[j]);
            }
            newEquation.push_back(resultStream.str());
            for (int j = i + 2; j < equation.size(); ++j)
            {
                newEquation.push_back(equation[j]);
            }
            equation = newEquation;
            return true;
        }
    }

    return false;
}

bool simplifyEquation(std::vector<std::string> & equation,
    const std::map<std::string, Account> & accounts
    )
{
    /*
    std::cout << "DEBUG simplifyEquation";
    for (const auto & i : equation)
    {
        std::cout << " " << i;
    }
    std::cout << std::endl;
    */

    for (int i = 0; i < equation.size(); ++i)
    {
        // Fetch account balance
        if (accounts.count(equation[i]) == 1)
        {
            std::vector<std::string> newequation;
            for (int j = 0; j < i ; ++j)
            {
                newequation.push_back(equation[j]);
            }

            std::ostringstream accountBalanceStream;
            accountBalanceStream << accounts.at(equation[i]).getBalance();
            std::string accountBalance = accountBalanceStream.str();
            newequation.push_back(accountBalance);
            for (int j = i + 1; j < equation.size(); ++j)
            {
                newequation.push_back(equation[j]);
            }
            equation = newequation;
            return true;

        }

        // Remove parentheses
        if (equation[i] == "(" and equation[i + 2] == ")")
        {
            std::vector<std::string> newequation;
            for (int j = 0; j < i ; ++j)
            {
                newequation.push_back(equation[j]);
            }

            newequation.push_back(equation[i + 1]);
            for (int j = i + 3; j < equation.size(); ++j)
            {
                newequation.push_back(equation[j]);
            }
            equation = newequation;
            return true;
        }
    }

    size_t start = 0;
    size_t end = equation.size() - 1;

    size_t parenthesisCounter(0);
    size_t max(0);
    bool closed = true;

    for (size_t i = 0; i < equation.size(); ++i)
    {
        if (equation[i] == "(")
        {
            ++parenthesisCounter;
            if (parenthesisCounter > max)
            {
                start = i;
                max = parenthesisCounter;
                closed = false;
            }
        }
        else if (equation[i] == ")")
        {
            if (not closed)
            {
                end = i;
                closed = true;
            }
            --parenthesisCounter;
        }
    }

    //std::cout << "DEBUG start= " << start << "  end= " << end << std::endl;

    std::string operation;

    // Priority: ^, *, /, +, -
    for (size_t i = start; i <= end; i++)
    {
        if (equation[i] == "^")
        {
            operation = equation[i];
            break;
        }
    }


    for (size_t i = start; i <= end; i++)
    {
        if (equation[i] == "*" or equation[i] == "/")
        {
            operation = equation[i];
            break;
        }
    }

    if (operation == "")
    {
        for (size_t i = start; i <= end; i++)
        {
            if (equation[i] == "+" or equation[i] == "-")
            {
                operation = equation[i];
                break;
            }
        }
    }

    if (simplifyOperation(operation, equation, start, end))
    {
        return true;
    }

    return false;
}

double parseAmount(const std::string & amountString,
    const std::map<std::string, Account> & accounts,
    const std::set<char> & simpleAmountCharacters
)
{
    //std::cout << "DEBUG  parseMathAmount  amountString= " << amountString << std::endl;


    std::ostringstream output;
    std::set<char> specialCharacters;
    specialCharacters.insert('(');
    specialCharacters.insert(')');
    specialCharacters.insert('+');
    specialCharacters.insert('-');
    specialCharacters.insert('/');
    specialCharacters.insert('*');
    specialCharacters.insert('^');

    for (size_t i = 0; i < amountString.length(); ++i)
    {
        char character = amountString[i];
        bool previousIsNumberCharacter(true);
        bool nextIsNumberCharacter(true);

        if (character == '-' and i != 0)
        {
            char previousCharacter = amountString[i - 1];
            if (previousCharacter != '-' and
                not simpleAmountCharacters.count(previousCharacter))
            {
                previousIsNumberCharacter = false;
            }
        }
        if (character == '-' and i != amountString.length() - 1)
        {
            char nextCharacter = amountString[i + 1];
            if (not simpleAmountCharacters.count(nextCharacter))
            {
                nextIsNumberCharacter = false;
            }
        }

        if (previousIsNumberCharacter and nextIsNumberCharacter and specialCharacters.count(character))
        {
            output << " " << character << " ";
        }
        else
        {
            output << character;
        }
    }

    std::string input = output.str();

    //std::cout << "DEBUG parseMathAmount  input= \"" << input << "\"" << std::endl;

    std::vector<std::string> equation;
    std::istringstream inputStream(input);

    while (not inputStream.eof())
    {
        std::string atom;
        inputStream >> atom;
        if (atom == "")
        {
            break;
        }
        equation.push_back(atom);
    }

    while (1)
    {
        bool result = simplifyEquation(equation, accounts);

        if (not result)
        {
            if (equation.size() == 1)
            {
                break;
            }

            std::cerr << "Error: simplification failed, input: " << input << std::endl;
            exit(1);
        }
    }

    std::istringstream resultStream(equation.front());
    double result;
    resultStream >> result;
    return result;
}

void trim(std::string & amountString)
{
    // Trim
    while (amountString.length() >= 1 and amountString[0] == ' ' or amountString[0] == '\t')
    {
        amountString = amountString.substr(1);
    }
    while (amountString.length() >= 1 and amountString[amountString.length() - 1] == ' ' or amountString[amountString.length() - 1] == '\t')
    {
        amountString = amountString.substr(0, amountString.length() - 1);
    }
}

void Posting::parseTransaction(
    const std::string & transactionLine,
    const std::map<std::string, Account> & accounts
)
{
    //std::cout << "DEBUG parseTransaction  transactionLine= " << transactionLine << std::endl;

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
            if (token.length() == 1)
            {
                //this is the last parenthesis
                break;
            }

            // CAD)
            currency = token.substr(0, token.length() - 1);
        }
        else
        {
            // CAD
            currency = token;
        }
    }


    std::string amountString = transactionLine;

    amountString.replace(amountString.find(accountName), accountName.length(), "");

    trim(amountString);

    if (amountString != "")
    {
        // /Find the last instance of the currency string
        //std::cout << "DEBUG Finding last occurrence of string " << currency << " in string " << amountString << std::endl;
        size_t currencyOffset = amountString.find(currency);

        if (currencyOffset == std::string::npos)
        {
            std::cout << "Error: amountString has no currency string" << std::endl;
            exit(1);
        }

        while (1)
        {
            size_t nextCurrencyOffset = amountString.find(currency, currencyOffset + currency.length());
            //std::cout << "DEBUG nextCurrencyOffset= " << nextCurrencyOffset << std::endl;
            if (nextCurrencyOffset == std::string::npos)
            {
                break;
            }
            currencyOffset = nextCurrencyOffset;
        }

        amountString.replace(currencyOffset, currency.length(), "");

        trim(amountString);
    }

/*
    std::cout << "DEBUG parseTransaction transactionLine= " << transactionLine;
    std::cout << "  accountName= " << accountName;
    std::cout << "  currency= " << currency;
    std::cout << "  amountString= \"" << amountString << "\"";
    std::cout << std::endl;
*/

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

    bool isMathLine(false);

    if (not isSimpleAmount and not isAutoLine)
    {
        isMathLine = true;
        std::set<char> mathAmountCharacters;
        mathAmountCharacters.insert('-');
        mathAmountCharacters.insert('0');
        mathAmountCharacters.insert('1');
        mathAmountCharacters.insert('2');
        mathAmountCharacters.insert('3');
        mathAmountCharacters.insert('4');
        mathAmountCharacters.insert('5');
        mathAmountCharacters.insert('6');
        mathAmountCharacters.insert('7');
        mathAmountCharacters.insert('8');
        mathAmountCharacters.insert('9');
        mathAmountCharacters.insert('.');
        mathAmountCharacters.insert(' ');
        mathAmountCharacters.insert('(');
        mathAmountCharacters.insert(')');

        for (size_t i = 0; i < amountString.length(); ++i)
        {
            char character = amountString[i];
            if (mathAmountCharacters.count(character) == 0)
            {
                isMathLine = false;
                break;
            }
        }
    }

    double amount;


    if (isSimpleAmount)
    {
        std::istringstream amountStream(amountString);
        amountStream >> amount;
    }
    else if (isAutoLine)
    {
        //std::cout << "DEBUG auto-line!" << std::endl;

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

            if (std::abs(balance2) >= MINIMUM_UNBALANCED_AMOUNT)
            {
                if (foundUnbalancedCurrency)
                {
                    std::cerr << "Error: posting can not contain 2 unbalanced currencies." << std::endl;
                }

                amount = -balance2;
                currency = currency2;
                //std::cout << "DEBUG auto-balancing, " << accountName << "  " << amount << " " << currency << std::endl;

                foundUnbalancedCurrency = true;
            }
        }

        if (not foundUnbalancedCurrency)
        {
            // Output any line.
            amount = 0;
            currency = m_transactions.front().getCurrency();
        }
    }
    else
    {
        amount = parseAmount(amountString, accounts, simpleAmountCharacters);
    }

    //std::cout << "DEBUG EMIT Transaction  " << accountName << "  " << amount << " " << currency << std::endl;
    Transaction transaction(*this, accountName, currency, amount);
    m_transactions.push_back(transaction);
}

void Posting::compute(std::map<std::string, Account> & accounts)
{
    generateTransactions(accounts);

    //std::cout << "DEBUG Posting::compute m_transactions = " << m_transactions.size() << std::endl;

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

        if (std::abs(balance) >= MINIMUM_UNBALANCED_AMOUNT)
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

        account.addTransaction(transaction);
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
