//
//  Transaction.cpp
//  dbg_hudson
//
//  Created by Jeongwon on 2/23/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

#include "Transaction.hpp"

Transaction::Transaction(const Execution* pExe, const boost::gregorian::date& dt, double capital): _pExe(pExe), _dt(dt), _capital(capital) {}

void Transaction::print(){
    std::cout << _pExe->symbol() << " ";
    _pExe->print();
}