cmake_minimum_required (VERSION 1.0)
project (seb-ledger)
set(CMAKE_CXX_FLAGS "-std=c++14")
add_executable(seb-ledger main.cpp LedgerFile.cpp Posting.cpp Transaction.cpp Account.cpp)

