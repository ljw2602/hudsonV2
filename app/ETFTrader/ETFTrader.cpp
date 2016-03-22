/*
 * Copyright (C) 2007, Alberto Giannetti
 *
 * This file is part of Hudson.
 *
 * Hudson is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hudson is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hudson.  If not, see <http://www.gnu.org/licenses/>.
 */

// STL
#include <vector>
#include <queue>
#include <algorithm>
#include <set>
#include <numeric>

// GSL
#include <gsl/gsl_vector.h>
#include <gsl/gsl_statistics.h>

// Hudson
#include "TA.hpp"
#include "EODBalanceSet.hpp"
#include "ETFTrader.hpp"

using namespace boost::gregorian;

/*
 TO DO: ENSURE ALL DB HAS THE SAME TIME STAMP, IF SO, MAKE A SERIES OF DT ONLY FOR TIME STAMP
        LEAVE EMPTY CELLS AS NULL?
        INCORPORATE DIVIDEND AND SPLIT INTO CONSIDERATION
 */

std::string ETFTrader::EQUITY("Equity");
std::string ETFTrader::TREASURY("Treasury");

ETFTrader::ETFTrader(const Series::EODSeries& equity_db,
                     const Series::EODSeries& treasury_db,
                     const Series::EODSeries& xly_db,
                     const Series::EODSeries& xlp_db,
                     const Series::EODSeries& xle_db,
                     const Series::EODSeries& xlf_db,
                     const Series::EODSeries& xlv_db,
                     const Series::EODSeries& xli_db,
                     const Series::EODSeries& xlb_db,
                     const Series::EODSeries& xlk_db,
                     const Series::EODSeries& xlu_db):
_num_trading(0)
{
    _benchmark.insert ( std::pair<const std::string, DB*>(EQUITY, new DB(equity_db)) );
    _benchmark.insert ( std::pair<const std::string, DB*>(TREASURY, new DB(treasury_db)) );
    
    _db.insert ( std::pair<const std::string, DB*>("XLY", new DB(xly_db)) );
    _db.insert ( std::pair<const std::string, DB*>("XLP", new DB(xlp_db)) );
    _db.insert ( std::pair<const std::string, DB*>("XLE", new DB(xle_db)) );
    _db.insert ( std::pair<const std::string, DB*>("XLF", new DB(xlf_db)) );
    _db.insert ( std::pair<const std::string, DB*>("XLV", new DB(xlv_db)) );
    _db.insert ( std::pair<const std::string, DB*>("XLI", new DB(xli_db)) );
    _db.insert ( std::pair<const std::string, DB*>("XLB", new DB(xlb_db)) );
    _db.insert ( std::pair<const std::string, DB*>("XLK", new DB(xlk_db)) );
    _db.insert ( std::pair<const std::string, DB*>("XLU", new DB(xlu_db)) );
    
    for (DBSet::const_iterator it = _db.begin(); it != _db.end(); it++) {
        _db_names.push_back(it->first);
    }
    
}


void run_FF_EOD_iterator(std::pair<const std::string, DB*>& p) {
    p.second->FF_EOD_iterator();
    return;
}

void run_FF_EOM_iterator(std::pair<const std::string, DB*>& p) {
    p.second->FF_EOM_iterator();
    return;
}

void run_advance_EOD_iterator(std::pair<const std::string, DB*>& p) {
    p.second->advance_EOD_iterator();
    return;
}
void run_advance_EOM_iterator(std::pair<const std::string, DB*>& p) {
    p.second->advance_EOM_iterator();
    return;
}


double yearly_return(TA ta, const Series::EOMSeries& EOMdb, Series::EOMSeries::const_iterator& EOM_iter){
    TA::ROCRes resROC = ta.ROC(EOMdb.adjclose(EOM_iter,13), 12);
    return resROC.roc[0];
}


struct request{
    enum Action {BUY, SELL, SELL_SHORT, COVER, CLOSE};
    Action type;
    std::string name;
    unsigned shares;
    request(Action type_, std::string name_, unsigned shares_): type(type_), name(name_), shares(shares_) {}
};

double ETFTrader::openPos_value(const boost::gregorian::date& dt, Series::EODDB::PriceType type) {
    PositionSet openPos = _miPositions.open();
    double value = 0;
    switch (type) {
        case Series::EODDB::OPEN:
            for (PositionSet::const_iterator it = openPos.begin(); it != openPos.end(); it++)
                value += _db.at((*it)->symbol())->daily.at(dt).open;
            break;
        case Series::EODDB::CLOSE:
            for (PositionSet::const_iterator it = openPos.begin(); it != openPos.end(); it++)
                value += _db.at((*it)->symbol())->daily.at(dt).close;
            break;
        case Series::EODDB::ADJCLOSE:
            for (PositionSet::const_iterator it = openPos.begin(); it != openPos.end(); it++)
                value += _db.at((*it)->symbol())->daily.at(dt).adjclose;
            break;
        default:
            break;
    }
    
    return value;
}

void ETFTrader::run(const std::string& initial_capital) throw(TraderException)
{
    TA ta;

    // Advance daily and monthly iterators to 1 year from beginning
    std::for_each(_db.begin(), _db.end(), run_FF_EOD_iterator);
    std::for_each(_db.begin(), _db.end(), run_FF_EOM_iterator);
    std::for_each(_benchmark.begin(), _benchmark.end(), run_FF_EOD_iterator);
    std::for_each(_benchmark.begin(), _benchmark.end(), run_FF_EOM_iterator);
    
    // Create a Set of union of dates, in case DBs have different set of dates
    // Set: An associative container that contains a sorted set of unique objects
    for (std::map<const std::string, DB*>::const_iterator it = _db.begin(); it != _db.end(); it++) {
        for (Series::EODSeries::const_iterator it2 = it->second->it_daily; it2 != it->second->daily.end(); it2++)
            date_set_daily.insert(it2->first);
    }
    for (std::map<const std::string, DB*>::const_iterator it = _db.begin(); it != _db.end(); it++) {
        for (Series::EOMSeries::const_iterator it2 = it->second->it_monthly; it2 != it->second->monthly.end(); it2++) {
            date_set_monthly.insert(it2->first);
        }
    }
    std::set<boost::gregorian::date>::const_iterator daily_iter = date_set_daily.begin();
    std::set<boost::gregorian::date>::const_iterator monthly_iter = date_set_monthly.begin();
    
    // Ensure all daily DBs have the same date as date_set_daily
    
    
    // Initialize bank roll
    EODBalanceSet::instance().initialize(*daily_iter, initial_capital, _db_names);
    std::cout << "Initial capital: $" << EODBalanceSet::instance().get_initial_capital() << std::endl;
    
    // Trading reservation
    std::queue< request > reservation;
    
    std::cout << "start" << std::endl;
    
    double latest_pos_value = 0;
    for(; daily_iter != date_set_daily.end(); daily_iter++ ) {
        try {
            boost::gregorian::date today = *daily_iter;
            
            // resolve reserved trading at open price
            while (! reservation.empty()) {
                
                request req = reservation.front();
                PositionSet ps = _miPositions.open(req.name);
                
                switch (req.type) {
                    case request::BUY:{
                        buy(req.name, today, Price(_db.at(req.name)->it_daily->second.open), req.shares);
                        _num_trading++;
                        break;
                    }
                        
                    case request::SELL:{
                        throw TraderException("Sell request is not valid in ETFTrader");
                        break;
                    }
                        
                    case request::SELL_SHORT:{
                        sell_short(req.name, today, Price(_db.at(req.name)->it_daily->second.open), req.shares);
                        break;
                    }
                        
                    case request::COVER:{
                        throw TraderException("Sell request is not valid in ETFTrader");
                        break;
                    }
                
                    case request::CLOSE:{
                        PositionPtr pPos = (*ps.begin());
                        close(pPos->id(), today, Price(_db.at(req.name)->it_daily->second.open));
                        break;
                    }
                        
                    default:
                        throw TraderException("Zero share trading request");
                }
                
                reservation.pop();
                latest_pos_value = openPos_value(today, Series::EODDB::OPEN);
            }
            
            // find buy & sell signals at each EOM
            if (today == *monthly_iter) {
                
                // 1-year return, find max
                std::map<const std::string, DB*>::const_iterator it = _db.begin();
                std::string max_return_name;
                double max_return = -std::numeric_limits<double>::max();
                for(; it != _db.end(); it++) {
                    double roc = yearly_return(ta, it->second->monthly, it->second->it_monthly);
                    if (roc > max_return) {
                        max_return_name = it->first;
                        max_return = roc;
                    }
                }
                std::cout << "Today: " << today << " " << max_return_name << " max return= " << max_return << "% (YOY)" << std::endl;
                
                double threshold = 3.0;
                // pick the best, reserve executions for tmr open,
                PositionSet ps = _miPositions.open();
                if ( ps.empty() && max_return > threshold ) {
                    unsigned nshares = 1;
                    reservation.push(request(request::BUY, max_return_name, nshares));
                }
                else if ( ps.empty() && max_return <= threshold ) {
                    
                }
                else if ( ps.size() == 1 && max_return > threshold ) {
                    PositionPtr pPos = (*ps.begin());
                    if ( pPos->symbol() != max_return_name ) {
                        unsigned nshares = 1;
                        reservation.push(request(request::CLOSE, pPos->symbol(), 0));
                        reservation.push(request(request::BUY, max_return_name, nshares));
                    }
                }
                else if ( ps.size() == 1 && max_return <= threshold ) {
                    PositionPtr pPos = (*ps.begin());
                    reservation.push(request(request::CLOSE, pPos->symbol(), 0));
                }
                else {
                    throw TraderException("More than one open position exist");
                }
                
                // forward DB monthly iterators
                monthly_iter++;
                std::for_each(_db.begin(), _db.end(), run_advance_EOM_iterator);
            }
            
            // EOD balance check for open positions
            
            PositionSet openPos = _miPositions.open();
            EODBalanceSet::instance().update_capital(today, openPos);
            
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            continue;
        }
        
        // forward DB daily iterators
        std::for_each(_db.begin(), _db.end(), run_advance_EOD_iterator);
    }

    std::cout << "end" << std::endl;
    
    return;
    
}

std::vector<double> ETFTrader::get_equity_monthly_ret(void) const {
    return _benchmark.at(EQUITY)->monthly_aggregate();
}

std::vector<double> ETFTrader::get_treasury_monthly_ret(void) const {
    return _benchmark.at(TREASURY)->monthly_resample();
}

void ETFTrader::summary(void) const {
    std::vector<double> portfolio_monthly_ret = EODBalanceSet::instance().monthly_ret(get_monthlyset());  // unit = %
    std::vector<double> equity_monthly_ret = get_equity_monthly_ret();  // unit = %
    std::vector<double> treausury_monthly_ret = get_treasury_monthly_ret();  // unit = %
    
    assert(portfolio_monthly_ret.size()==equity_monthly_ret.size());
    for (int i = 0; i < portfolio_monthly_ret.size(); i++) {
        std::cout << portfolio_monthly_ret[i] << " " << equity_monthly_ret[i] << " " << treausury_monthly_ret[i] << std::endl;
        portfolio_monthly_ret[i] -= treausury_monthly_ret[i];
        equity_monthly_ret[i] -= treausury_monthly_ret[i];
    }
    
    gsl_vector_const_view gsl_x = gsl_vector_const_view_array(&equity_monthly_ret[0], equity_monthly_ret.size());
    gsl_vector_const_view gsl_y = gsl_vector_const_view_array(&portfolio_monthly_ret[0], portfolio_monthly_ret.size());
    double varx = gsl_stats_variance(gsl_x.vector.data, 1, equity_monthly_ret.size());
    double covar = gsl_stats_covariance(gsl_x.vector.data, 1, gsl_y.vector.data, 1, portfolio_monthly_ret.size());
    double beta = covar/varx;
    
    double ybar = std::accumulate(equity_monthly_ret.begin(), equity_monthly_ret.end(), 0.0)/equity_monthly_ret.size();
    double xbar = std::accumulate(portfolio_monthly_ret.begin(), portfolio_monthly_ret.end(), 0.0)/portfolio_monthly_ret.size();
    double alpha = ybar - beta*xbar;
    
    double y_sq_sum = std::inner_product(equity_monthly_ret.begin(), equity_monthly_ret.end(), equity_monthly_ret.begin(), 0.0);
    double ystdev = std::sqrt(y_sq_sum / equity_monthly_ret.size() - ybar * ybar);
    double sharpe = ybar / ystdev;
    
    std::cout << "Beta: " << beta << std::endl;
    std::cout << "Alpha: " << alpha << std::endl;
    std::cout << "Sharpe: " << sharpe << std::endl;
}
