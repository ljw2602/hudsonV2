//
//  TransactionSet.h
//  dbg_hudson
//
//  Created by Jeongwon on 2/23/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

#ifndef __dbg_hudson__TransactionSet__
#define __dbg_hudson__TransactionSet__

// Boost
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

// Hudson
#include "TransactionPtr.hpp"


typedef boost::multi_index::multi_index_container <
TransactionPtr,
boost::multi_index::indexed_by <
boost::multi_index::ordered_non_unique
<
boost::multi_index::const_mem_fun<Transaction,
boost::gregorian::date,
&Transaction::get_date>
>
>
> __TransactionSet;


class TransactionSet: private __TransactionSet {
public:
    using __TransactionSet::insert;
    using __TransactionSet::const_iterator;
    
    void transact(const Execution* pExe, const boost::gregorian::date& dt, double capital);
    void print(unsigned) const;
};

#endif /* defined(__dbg_hudson__TransactionSet__) */
