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


class EODBalanceException: public std::exception
{
public:
    EODBalanceException(const std::string& msg):
    _Str("EODBalanceException: ")
    {
        _Str += msg;
    }
    
    virtual ~EODBalanceException(void) throw() { }
    virtual const char *what(void) const throw() { return _Str.c_str(); }
    
protected:
    std::string _Str;
};


struct EODBalance {
    std::string name;
    boost::gregorian::date dt;
    double balance;
    EODBalance(std::string& name_, boost::gregorian::date dt_, double balance_): name(name_), dt(dt_), balance(balance_) {}
    EODBalance(const std::string& name_, const boost::gregorian::date dt_, const double balance_): name(name_), dt(dt_), balance(balance_) {}
    void print() {
        std::cout << name << "\t" << dt << "\t" << balance << std::endl;
    }
};

#endif /* defined(__dbg_hudson__Transaction__) */
