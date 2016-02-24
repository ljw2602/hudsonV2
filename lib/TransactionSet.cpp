//
//  TransactionSet.cpp
//  dbg_hudson
//
//  Created by Jeongwon on 2/23/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

#include "TransactionSet.hpp"

void TransactionSet::transact(const Execution* pExe, const boost::gregorian::date& dt, double capital) {
    if (!empty() && dt < (*rbegin())->get_date()) {
        throw TransactionException("Current transaction is not the latest one");
    }
    
    TransactionPtr pTrans(new Transaction(pExe, dt, capital));
    if (insert(pTrans).second == false)
        throw TransactionException("Cannot insert transaction in TransactionSet");
}

void TransactionSet::print(unsigned digit) const {
    for (const_iterator iter = begin(); iter != end(); iter++) {
        std::cout << std::setfill(' ') << std::setw(digit+5) << std::fixed << std::setprecision(2) << (*iter)->get_capital() << " after ";
        (*iter)->print();
        std::cout << std::endl;
    }
}
