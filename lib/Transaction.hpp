//
//  Transaction.h
//  dbg_hudson
//
//  Created by Jeongwon on 2/23/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

#ifndef __dbg_hudson__Transaction__
#define __dbg_hudson__Transaction__

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include "ExecutionSet.hpp"


class TransactionException: public std::exception
{
public:
    TransactionException(const std::string& msg):
    _Str("TransactionException: ")
    {
        _Str += msg;
    }
    
    virtual ~TransactionException(void) throw() { }
    virtual const char *what(void) const throw() { return _Str.c_str(); }
    
protected:
    std::string _Str;
};


class Transaction {
public:
    Transaction(const Execution* pExe, const boost::gregorian::date& dt, double capital);
    
    boost::gregorian::date get_date(void) const { return _dt; }  // for multi-index sorting
    double get_capital(void) const { return _capital; }
    void print();

private:
    const Execution* _pExe;
    const boost::gregorian::date _dt;
    double _capital;  // capital after this transaction
};

#endif /* defined(__dbg_hudson__Transaction__) */
