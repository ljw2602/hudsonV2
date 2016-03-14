//
//  EODBalance.cpp
//  dbg_hudson
//
//  Created by Jeongwon on 2/19/16.
//  Copyright (c) 2016 Jeongwon. All rights reserved.
//

// STL
#include <string>
#include <fstream>

// Hudson
#include <EODDB.hpp>
#include "EODBalanceSet.hpp"


std::auto_ptr<EODBalanceSet> EODBalanceSet::_pInstance;
std::string EODBalanceSet::CASH("Cash");
std::string EODBalanceSet::TOTAL("Total");


EODBalanceSet& EODBalanceSet::instance(void) {
    if ( _pInstance.get() == 0)
        _pInstance.reset(new EODBalanceSet());  // set to -1 to present it is not initialized
    return *_pInstance;
}


void EODBalanceSet::initialize(const boost::gregorian::date& dt, const std::string& init_, const std::vector<std::string>& names_) {
    
    names = names_;
    first_trading_dt = dt;
    
    digit = init_.size();
    double init = std::atof(init_.c_str());
    
    // init amount has to be positive
    if(init < 0)
        throw EODBalanceException("Negative base capital");
    
    _initial_capital = init;
    
    for (std::vector<std::string>::const_iterator it = names_.begin(); it != names_.end(); it++) {
        EODBalancePtr pEODBalance( new EODBalance(*it, dt, 0) );
        insert(pEODBalance);
    }
    
    EODBalancePtr pCASH(new EODBalance(CASH, dt, _initial_capital));
    insert(pCASH);
    EODBalancePtr pTOTAL(new EODBalance(TOTAL, dt, _initial_capital));
    insert(pTOTAL);

//    std::cout << "Initialized" << std::endl;
//    for (const_iterator it = begin(); it != end(); it++) {
//        (*it)->print();
//    }
}


double EODBalanceSet::get_initial_capital(void) const {
    return _initial_capital;
}


struct update_balance {
    update_balance(double new_balance_) : new_balance(new_balance_){}
    
    void operator()(boost::shared_ptr<EODBalance> pEOD)
    {
        pEOD->balance = new_balance;
    }
    
private:
    double new_balance;
};

double EODBalanceSet::_update_capital(const std::string& name, const boost::gregorian::date& dt, const double diff){
    // 1. search name
    // 2. if exists, add diff / if not, insert new dt
    // 2. its date has to be either equal to or smaller than pExe->dt()

    if(empty())
        throw EODBalanceException("EODBalanceSet empty; cannot update capital");
    
    try {
        name_pair its = equal_range(name);
        iterator it = its.second; it--;

        const boost::gregorian::date last_date = (*it)->dt;
        const double last_value = (*it)->balance;
        
        if (last_date == dt) {
            // if already exists, modify the balance
            modify(it, update_balance(last_value + diff));
        } else {
            // if not, insert one
            EODBalancePtr pEODBalance(new EODBalance(name, dt, last_value + diff));
            insert(pEODBalance);
        }
        
        return last_value + diff;

    } catch (std::exception& ex ) {
        
        std::cerr << ex.what() << std::endl;
        exit(EXIT_FAILURE);
        
    }

}

void EODBalanceSet::update_capital(const Execution* pExe){

    double diff;
    if (pExe->action() == "Bought") {
        diff = -(pExe->price() * pExe->size());
    }
    else if (pExe->action() ==  "Sold") {
        diff = pExe->price() * pExe->size();
    }
    else if (pExe->action() ==  "Shorted") {
        diff = pExe->price() * pExe->size();
    }
    else if (pExe->action() ==   "Covered") {
        diff = -(pExe->price() * pExe->size());
    }
    else {
        throw EODBalanceException("Unknown type of execution");
    }
    
    _update_capital(CASH, pExe->dt(), diff);
    
//    for (const_iterator it = begin(); it != end(); it++) {
//        (*it)->print();
//    }

    return;
}

void EODBalanceSet::update_capital(const boost::gregorian::date& dt, const PositionSet& openPos){
    // 1. Cash update
    // 2. Open position EOD update

    double EOD_total = 0;
    
    EOD_total += _update_capital(CASH, dt, 0);
    
    for (int i = 0; i < names.size(); i++) {
        
        std::string name = names[i];
        PositionSet::by_symbol::const_iterator symbol_key_end = openPos.get<1>().find(name);
        
        double eod_vaue;
        if (symbol_key_end == openPos.get<1>().end()) {
            // if name is not in open position, add 0
            eod_vaue = 0;
            EODBalancePtr pEODBalance(new EODBalance(name, dt, eod_vaue));
            insert(pEODBalance);
        } else {
            // else, add close price
            eod_vaue = Series::EODDB::instance().get(name).at(dt).close;
            EODBalancePtr pEODBalance(new EODBalance(name, dt, eod_vaue));
            insert(pEODBalance);
        }
        EOD_total += eod_vaue;
    }

    EODBalancePtr pTotal(new EODBalance(TOTAL, dt, EOD_total));
    insert(pTotal);
    
//    std::cout << "EOD summary" << std::endl;
//    for (const_iterator it = begin(); it != end(); it++) {
//        (*it)->print();
//    }
    
    return;
}


void EODBalanceSet::print(void) const {
    // Print total EODBalance value
    name_pair its = equal_range(TOTAL);
    for (EODBalanceSet::const_iterator it = its.first; it != its.second; it++) {
        (*it)->print();
    }

    return;
}

void EODBalanceSet::export_to_csv(void) const {
    std::ofstream myfile;
    myfile.open ("EODBalance.csv");
    
    // Header
    myfile << "Date";
    for (int i = 0; i < names.size(); i++) {
        myfile << "," << names[i];
    }
    myfile << "," << CASH << "," << TOTAL << "\n";

    
    // Content
    by_date::const_iterator it_date = get<1>().begin();
    by_date::const_iterator it_end = get<1>().end();
    boost::gregorian::date date;
    
    while (it_date != it_end) {
        date = (*it_date)->dt;
        date_pair its = by_date::equal_range(date);
        std::map<std::string, double> temp;
        for (by_date::const_iterator it = its.first; it != its.second; it++) {
            temp.insert( std::pair<std::string, double>((*it)->name, (*it)->balance) );
        }
        
        myfile << date << ",";
        for (int i = 0; i < names.size(); i++) {
            myfile << temp.at(names[i]) << ",";
        }
        myfile << temp.at(CASH) << "," << temp.at(TOTAL) << "\n";
        
        it_date = its.second;
    }
    
    myfile.close();
    return;
}
