//
//  DB.h
//  dbg_hudson
//
//  Created by Jeongwon on 3/10/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

#ifndef dbg_hudson_DB_h
#define dbg_hudson_DB_h

class DB{
public:
    DB(const Series::EODSeries& db_) : daily(db_), monthly(db_.monthly()) {
        it_daily = daily.begin();
        it_monthly = monthly.begin();
    }
    void FF_EOD_iterator() {
        it_daily = daily.last_in_month(it_daily->first.year()+1, it_daily->first.month());
        if (it_daily == daily.end())
            throw TraderException("1 year fast-forward EOD iteration failed");
        return;
    }
    void FF_EOM_iterator() {
        while ( !(it_monthly->first.year()==it_daily->first.year() && it_monthly->first.month()==it_daily->first.month()) && it_monthly != monthly.end())
            it_monthly++;
        if (it_monthly == monthly.end())
            throw TraderException("1 year fast-forward EOM iteration failed");
        return;
    }
    void advance_EOD_iterator() {
        it_daily++;
        return;
    }
    void advance_EOM_iterator() {
        it_monthly++;
        return;
    }
    
public:
    const Series::EODSeries& daily;
    Series::EOMSeries monthly;
    Series::EODSeries::const_iterator it_daily;
    Series::EOMSeries::const_iterator it_monthly;
};

#endif
