//
//  Header.h
//  dbg_hudson
//
//  Created by Jeongwon on 3/16/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

#ifndef dbg_hudson_Header_h
#define dbg_hudson_Header_h

#include <iostream>
#include <string>

#include "boost/date_time/gregorian/gregorian.hpp"

class Asset {
public:
    Asset(std::string symbol_): symbol(symbol_) {}
    
    const Series::EODSeries& getDB() {
        const Series::EODSeries& db = Series::EODDB::instance().get(symbol);
        std::cout << symbol << " Records: " << db.size() << std::endl;
        std::cout << symbol << " Period: " << db.period() << std::endl;
        std::cout << symbol << " Total days: " << db.duration().days() << std::endl;
        return db;
    }
    
protected:
    const std::string symbol;
};

class EquityBenchmark: public Asset{
public:
    EquityBenchmark(std::string filepath, std::string symbol_, boost::gregorian::date load_begin, boost::gregorian::date load_end): Asset(symbol_) {
        std::cout << "Loading " << filepath << " from " << to_simple_string(load_begin) << " to " << to_simple_string(load_end) << "..." << std::endl;
        Series::EODDB::instance().load(symbol, filepath, Series::EODDB::EQUITYBENCHMARK, load_begin, load_end);
    }
};


class TreasuryBenchmark: public Asset{
public:
    TreasuryBenchmark(std::string filepath, std::string symbol_, boost::gregorian::date load_begin, boost::gregorian::date load_end): Asset(symbol_) {
        std::cout << "Loading " << filepath << " from " << to_simple_string(load_begin) << " to " << to_simple_string(load_end) << "..." << std::endl;
        Series::EODDB::instance().load(symbol, filepath, Series::EODDB::TREASURYBENCHMARK, load_begin, load_end);
    }
};


class ETF: public Asset{
public:
    ETF(std::string filepath, std::string symbol_, boost::gregorian::date load_begin, boost::gregorian::date load_end): Asset(symbol_) {
        std::cout << "Loading " << filepath << " from " << to_simple_string(load_begin) << " to " << to_simple_string(load_end) << "..." << std::endl;
        Series::EODDB::instance().load(symbol, filepath, Series::EODDB::YAHOO, load_begin, load_end);
    }
};

#endif
