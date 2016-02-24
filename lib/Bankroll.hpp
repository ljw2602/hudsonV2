//
//  Bankroll.h
//  dbg_hudson
//
//  Created by Jeongwon on 2/19/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

#ifndef __dbg_hudson__Bankroll__
#define __dbg_hudson__Bankroll__

// STL
#include <memory>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include "Price.hpp"
#include "ExecutionSet.hpp"
#include "TransactionSet.hpp"

/* Note : why not have another ExecutionSet that tracks the entire Executions?
 1) it adds any Executions. no date filtering
 2) no way of adding capital */

class BankrollException: public std::exception
{
public:
    BankrollException(const std::string& msg):
    _Str("BankrollException: ")
    {
        _Str += msg;
    }
    
    virtual ~BankrollException(void) throw() { }
    virtual const char *what(void) const throw() { return _Str.c_str(); }
    
protected:
    std::string _Str;
};


class Bankroll{
public:
    static Bankroll& instance(void);
    
    void set_initial_capital(const std::string&);
    double get_initial_capital(void) const { return _initial_capital; }

    void update_capital(const Execution* pExe);

    void print(void) const;
    
private:
    static std::auto_ptr<Bankroll> _pInstance;
    Bankroll(double capital_) : capital(capital_), _initial_capital(capital_) {}

    double capital;
    double _initial_capital;
    unsigned digit;
    TransactionSet _miTransactions;
};

#endif /* defined(__dbg_hudson__Bankroll__) */
