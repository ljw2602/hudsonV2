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
#include <map>
#include <utility>
#include <string>
#include <vector>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

// Hudson
#include "Price.hpp"
#include "ExecutionSet.hpp"
#include "EODBalancePtr.hpp"
#include "PositionSet.hpp"


typedef boost::multi_index::multi_index_container <
    EODBalancePtr,
    boost::multi_index::indexed_by <
        boost::multi_index::ordered_non_unique
        <
            boost::multi_index::member
            <
                EODBalance, std::string, &EODBalance::name
            >
        >,
        boost::multi_index::ordered_non_unique
        <
            boost::multi_index::tag<date_key>,
            boost::multi_index::member
            <
                EODBalance, boost::gregorian::date, &EODBalance::dt
            >
        >
    >
> __EODBalanceSet;


class EODBalanceSet: private __EODBalanceSet {
public:
    using __EODBalanceSet::insert;
    using __EODBalanceSet::iterator;
    using __EODBalanceSet::const_iterator;
    using __EODBalanceSet::equal_range;
    using __EODBalanceSet::modify;
    using __EODBalanceSet::size;
    
    static EODBalanceSet& instance(void);

    void initialize(const boost::gregorian::date& dt, const std::string&, const std::vector<std::string>&);
    double get_initial_capital(void) const;
    
    double _update_capital(const std::string& name, const boost::gregorian::date&, const double); // inner function
    void update_capital(const Execution* pExe);  // this one is for adding new execution
    void update_capital(const boost::gregorian::date&, const PositionSet& openPos); // EOD

    void print(void) const;
    std::map<boost::gregorian::date, double> monthly(const std::set<boost::gregorian::date>&) const;
    std::vector<double> monthly_ret(const std::set<boost::gregorian::date>&) const;
    void export_to_csv(void) const;
    
private:
    static std::auto_ptr<EODBalanceSet> _pInstance;

    static std::string CASH, TOTAL;
    double _initial_capital;
    unsigned digit;
    std::vector<std::string> names;
    boost::gregorian::date first_trading_dt;
    
    typedef __EODBalanceSet::index<date_key>::type by_date;
    typedef std::pair<__EODBalanceSet::const_iterator, __EODBalanceSet::const_iterator> name_pair;
    typedef std::pair<by_date::const_iterator, by_date::const_iterator> date_pair;
};

#endif /* defined(__dbg_hudson__Bankroll__) */
