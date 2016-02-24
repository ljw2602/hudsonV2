//
//  Bankroll.cpp
//  dbg_hudson
//
//  Created by Jeongwon on 2/19/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

#include <string>

#include "Bankroll.hpp"

std::auto_ptr<Bankroll> Bankroll::_pInstance;

Bankroll& Bankroll::instance(void) {
    if ( _pInstance.get() == 0)
        _pInstance.reset(new Bankroll(-1));
    return *_pInstance;
}

void Bankroll::set_initial_capital(const std::string& init_) {
    digit = init_.size();
    double init = std::atof(init_.c_str());
    
    if(init < 0)
        throw BankrollException("Negative base capital");
    if (capital > 0)
        throw BankrollException("Bankroll already initialized");
    
    capital = init;
    _initial_capital = init;
}

void Bankroll::update_capital(const Execution* pExe){
    
    if (pExe->action() == "Bought" || pExe->action() ==   "Covered") {
        capital -= pExe->price() * pExe->size();
    }
    else if (pExe->action() ==  "Sold" || pExe->action() ==  "Shorted") {
        capital += pExe->price() * pExe->size();
    }
//    else if (pExe->action() ==  "Shorted") {
//        capital += pExe->price() * pExe->size();
//    }
//    else if (pExe->action() ==   "Covered") {
//        capital -= pExe->price() * pExe->size();
//    }
    else {
        throw BankrollException("Unknown type of execution");
    }
    
    _miTransactions.transact(pExe, pExe->dt(), capital);
    
    if (capital < 0) {
        print();
        throw BankrollException("Capital goes to negative");
    }
    
}

void Bankroll::print(void) const {
    std::cout << std::setfill(' ') << std::setw(digit+5) << std::fixed << std::setprecision(2) << _initial_capital << " initial capital" << std::endl;
    _miTransactions.print(digit);
}
