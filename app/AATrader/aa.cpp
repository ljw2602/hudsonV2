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

#include "StdAfx.h"

// STL
#include <sstream>

// Boost
#include <boost/program_options.hpp>

// Hudson
#include <EODDB.hpp>
#include <PositionFactors.hpp>
#include <PositionFactorsSet.hpp>
#include <BnHTrader.hpp>
#include <EOMReturnFactors.hpp>
#include <EOMReport.hpp>
#include <PositionsReport.hpp>
#include <PortfolioReturns.hpp>
#include <PortfolioReport.hpp>

#include "AATrader.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;

namespace po = boost::program_options;


int main(int argc, char* argv[])
{
  string begin_date, end_date;
  string spx_dbfile, tnx_dbfile, djc_dbfile, eafe_dbfile, reit_dbfile;

  try {

    /*
     * Extract simulation options
     */
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("spx_file",   po::value<string>(&spx_dbfile),     "SPX series database")
      ("tnx_file",   po::value<string>(&tnx_dbfile),     "TNX series database")
      ("djc_file",   po::value<string>(&djc_dbfile),     "DJC series database")
      ("eafe_file",  po::value<string>(&eafe_dbfile),    "EAFE series database")
      ("reit_file",  po::value<string>(&reit_dbfile),    "REIT series database")
      ("begin_date", po::value<string>(&begin_date),     "start of trading period (YYYY-MM-DD)")
      ("end_date",   po::value<string>(&end_date),       "end of trading period (YYYY-MM-DD)")
      ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if( vm.count("help") ) {
      cout << desc << endl;
      exit(0);
    }

    if( vm["spx_file"].empty() ||
        vm["tnx_file"].empty() ||
        vm["djc_file"].empty() ||
        vm["eafe_file"].empty() ||
        vm["reit_file"].empty() ||
	      vm["begin_date"].empty() || vm["end_date"].empty() ) {
      cout << desc << endl;
      exit(1);
    }

    /*
     * Load series data
     */
    date load_begin(from_simple_string(begin_date));
    if( load_begin.is_not_a_date() ) {
      cerr << "Invalid begin date " << begin_date << endl;
      exit(EXIT_FAILURE);
    }

    date load_end(from_simple_string(end_date));
    if( load_end.is_not_a_date() ) {
      cerr << "Invalid end date " << end_date << endl;
      exit(EXIT_FAILURE);
    }

    const string spx_symbol = "SPX";
    const string tnx_symbol = "TNX";
    const string reit_symbol = "REIT";
    const string djc_symbol = "DJC";
    const string eafe_symbol = "EAFE";
    
    cout << "Loading " << spx_dbfile << " from " << load_begin << " to " << load_end << "..." << endl;
    Series::EODDB::instance().load(spx_symbol, spx_dbfile, Series::EODDB::YAHOO, load_begin, load_end);
    
    cout << "Loading " << tnx_dbfile << " from " << load_begin << " to " << load_end << "..." << endl;
    Series::EODDB::instance().load(tnx_symbol, tnx_dbfile, Series::EODDB::YAHOO, load_begin, load_end);
    
    cout << "Loading " << djc_dbfile << " from " << load_begin << " to " << load_end << "..." << endl;
    Series::EODDB::instance().load(djc_symbol, djc_dbfile, Series::EODDB::YAHOO, load_begin, load_end);
    
    cout << "Loading " << eafe_dbfile << " from " << load_begin << " to " << load_end << "..." << endl;
    Series::EODDB::instance().load(eafe_symbol, eafe_dbfile, Series::EODDB::DMYC, load_begin, load_end);

    cout << "Loading " << reit_dbfile << " from " << load_begin << " to " << load_end << "..." << endl;
    Series::EODDB::instance().load(reit_symbol, reit_dbfile, Series::EODDB::DMYC, load_begin, load_end);

    const Series::EODSeries& spx_db = Series::EODDB::instance().get(spx_symbol);
    const Series::EODSeries& tnx_db = Series::EODDB::instance().get(tnx_symbol);
    const Series::EODSeries& djc_db = Series::EODDB::instance().get(djc_symbol);
    const Series::EODSeries& reit_db = Series::EODDB::instance().get(reit_symbol);
    const Series::EODSeries& eafe_db = Series::EODDB::instance().get(eafe_symbol);
        
    cout << "SPX Records: " << spx_db.size() << endl;
    cout << "SPX Period: " << spx_db.period() << endl;
    cout << "SPX Total days: " << spx_db.duration().days() << endl;

    cout << "TNX Records: " << tnx_db.size() << endl;
    cout << "TNX Period: " << tnx_db.period() << endl;
    cout << "TNX Total days: " << tnx_db.duration().days() << endl;
    
    cout << "DJC Records: " << djc_db.size() << endl;
    cout << "DJC Period: " << djc_db.period() << endl;
    cout << "DJC Total days: " << djc_db.duration().days() << endl;

    cout << "EAFE Records: " << eafe_db.size() << endl;
    cout << "EAFE Period: " << eafe_db.period() << endl;
    cout << "EAFE Total days: " << eafe_db.duration().days() << endl;

    cout << "REIT Records: " << reit_db.size() << endl;
    cout << "REIT Period: " << reit_db.period() << endl;
    cout << "REIT Total days: " << reit_db.duration().days() << endl;
    
    // Initialize and run strategy
    AATrader trader(spx_db, tnx_db, djc_db, eafe_db, reit_db);
    trader.run();

    // Print trades  
    Report::header("Closed trades");
    trader.positions("SPX").closed().print();
    trader.positions("TNX").closed().print();
    trader.positions("DJC").closed().print();
    trader.positions("EAFE").closed().print();
    trader.positions("REIT").closed().print();
    
    Report::header("Open trades");
    trader.positions("SPX").open().print();
    trader.positions("TNX").open().print();
    trader.positions("DJC").open().print();
    trader.positions("EAFE").open().print();
    trader.positions("REIT").open().print();
            
    // SPX stats
    Report::header("SPX Stats");
    EOMReturnFactors spx_eomrf(trader.positions("SPX"), load_begin, load_end);
    EOMReport rp(spx_eomrf);
    rp.print();
    
    // TNX stats
    Report::header("TNX Stats");
    EOMReturnFactors tnx_eomrf(trader.positions("TNX"), load_begin, load_end);
    EOMReport tnx_rp(tnx_eomrf);
    tnx_rp.print();
    
    // DJC stats
    Report::header("DJC Stats");
    EOMReturnFactors djc_eomrf(trader.positions("DJC"), load_begin, load_end);
    EOMReport djc_rp(djc_eomrf);
    djc_rp.print();
    
    // EAFE stats
    Report::header("EAFE Stats");
    EOMReturnFactors eafe_eomrf(trader.positions("EAFE"), load_begin, load_end);
    EOMReport eafe_rp(eafe_eomrf);
    eafe_rp.print();
    
    // REIT stats
    Report::header("REIT Stats");
    EOMReturnFactors reit_eomrf(trader.positions("REIT"), load_begin, load_end);
    EOMReport reit_rp(reit_eomrf);
    reit_rp.print();
    
    // Position analysis
    //Report::header("Positions");
    //PositionFactorsSet pfs(trader.positions());
    //PositionsReport pr(pfs);
    //pr.print();
    
    // Portfolio stats
    Report::header("Portfolio Stats");
    PortfolioReturns pr;
    pr.add(&spx_eomrf);
    pr.add(&tnx_eomrf);
    pr.add(&djc_eomrf);
    pr.add(&eafe_eomrf);
    pr.add(&reit_eomrf);
    PortfolioReport preport(pr);
    preport.print();

    // BnH
    Report::header("SPX BnH");
    BnHTrader bnh(spx_db); 
    bnh.run();
    bnh.positions().print();
    EOMReturnFactors bnh_eomrf(bnh.positions(), load_begin, load_end);
    EOMReport bnh_rp(bnh_eomrf);
    bnh_rp.print();

  } catch( std::exception& ex ) {

    cerr << ex.what() << endl;
    exit(EXIT_FAILURE);
  }

  return 0;
}
