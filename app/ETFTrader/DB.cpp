//
//  DB.cpp
//  dbg_hudson
//
//  Created by Jeongwon on 3/18/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

#include <numeric>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include "DB.hpp"

double cumreturn( double result, const std::pair<boost::gregorian::date, Series::DayPrice>& obj )
{
    return result * ( 1.0 + obj.second.close/100.0 );
}

std::vector<double> DB::monthly_aggregate() const {
    /*
     S&P 500 return
     xi = daily return in percentage
     return = cumulative monthly return ((Product of (1+xi/100)) - 1) * 100
     */
    Series::EODSeries::const_iterator it, it2;
    it = it_daily; it++; // EODSeries starts at month end. Forward it to go to the next month
    it2 = it;
    
    boost::gregorian::date::month_type current_mon = it->first.month();
    std::vector<double> result;
    while (1) {
        while (it2 != daily.end() && current_mon == it2->first.month()) {
            it2++;
        }
        if (it2 != daily.end()) {
            double cumret = std::accumulate<Series::EODSeries::const_iterator, double>(it, it2, 1, cumreturn);
            //std::cout << current_mon << " " << cumret << std::endl;
            result.push_back((cumret-1.0)*100.0);
            
            it = it2;
            current_mon = it->first.month();
        } else {
            double cumret = std::accumulate<Series::EODSeries::const_iterator, double>(it, it2, 1, cumreturn);
            result.push_back((cumret-1.0)*100.0);
            //std::cout << current_mon << " " << cumret << std::endl;
            break;
        }
    }
    return result;
}

std::vector<double> DB::monthly_resample() const {
    /*
     1MO T-bill yield, annualized
     xi = monthly return in percentage
     return  = BOM value / 12
     */
    Series::EODSeries::const_iterator it, it2;
    it = it_daily; it++; // EODSeries starts at month end. Forward it to go to the next month
    it2 = it;
    
    boost::gregorian::date::month_type current_mon = it->first.month();
    std::vector<double> result;
    while (1) {
        while (it2 != daily.end() && current_mon == it2->first.month()) {
            it2++;
        }
        if (it2 != daily.end()) {
            //double cumret = std::accumulate<Series::EODSeries::const_iterator, double>(it, it2, 1, cumreturn);
            //std::cout << current_mon << " " << cumret << std::endl;
            //result.push_back((cumret-1.0)*100.0);
            result.push_back(it->second.close/12.0);
            
            it = it2;
            current_mon = it->first.month();
        } else {
            //double cumret = std::accumulate<Series::EODSeries::const_iterator, double>(it, it2, 1, cumreturn);
            //result.push_back((cumret-1.0)*100.0);
            //std::cout << current_mon << " " << cumret << std::endl;
            it2--;
            result.push_back(it->second.close*(it2->first.day()/30.0)/12.0);
            break;
        }
    }
    return result;
}