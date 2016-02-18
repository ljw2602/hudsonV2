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

// Hudson
#include "PortfolioReturns.hpp"

using namespace std;


PortfolioReturns::PortfolioReturns( void ):
  _accWeight(0)
{
}


void PortfolioReturns::add( EOMReturnFactors* rf, double weight) throw(PortfolioReturnsException)
{
  if( weight < 0 )
    throw PortfolioReturnsException("Invalid EOM return factors weight");
    
  if( (_accWeight + weight) > 1 )
    throw PortfolioReturnsException("EOM return factors weight exceeding portfolio size");
    
  _accWeight += weight;
  
  EOMRFWeight rfw(rf, weight);
    
  _vRF.push_back(rfw);
}


double PortfolioReturns::roi( void ) const
{
  if( _vRF.empty() )
    return 0;

  double acc_cagr = 0;
  for( size_t i = 0; i < _vRF.size(); ++i )
    acc_cagr += (_vRF[i].pEOMRF->roi() * (_accWeight ? _vRF[i].w : 1));

  return acc_cagr / (_accWeight ? 1 : _vRF.size());
}


double PortfolioReturns::cagr( void ) const
{
  if( _vRF.empty() )
    return 0;

  double acc_cagr = 0;
  for( size_t i = 0; i < _vRF.size(); ++i )
    acc_cagr += (_vRF[i].pEOMRF->cagr() * (_accWeight ? _vRF[i].w : 1));

  return acc_cagr / (_accWeight ? 1 : _vRF.size());
}


double PortfolioReturns::gsd( void ) const
{
  if( _vRF.empty() )
    return 0;

  double acc_cagr = 0;
  for( size_t i = 0; i < _vRF.size(); ++i )
    acc_cagr += (_vRF[i].pEOMRF->gsd() * (_accWeight ? _vRF[i].w : 1));

  return acc_cagr / (_accWeight ? 1 : _vRF.size());
}


double PortfolioReturns::sharpe( void ) const
{
  if( _vRF.empty() )
    return 0;

  double acc_sharpe = 0;
  for( size_t i = 0; i < _vRF.size(); ++i )
    acc_sharpe += (_vRF[i].pEOMRF->sharpe() * (_accWeight ? _vRF[i].w : 1));

  return acc_sharpe / (_accWeight ? 1 : _vRF.size());
}


PortfolioReturns::EOMRFWeight::EOMRFWeight( EOMReturnFactors* pEOMRF_, double w_ ):
  pEOMRF(pEOMRF_),
  w(w_)
{
}
