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
#include <vector>

// Hudson
#include "TA.hpp"

using namespace std;


TA::TA(void) throw(TAException)
{
  TA_RetCode ta_ret;

  if( (ta_ret = TA_Initialize()) != TA_SUCCESS )
    throw TAException(getError(ta_ret));
}


TA::~TA( void )
{
  TA_Shutdown();
}


TA::SMARes TA::SMA(vDouble vSeries, unsigned ma_period) const throw(TAException)
{
  if( vSeries.size() < ma_period )
    throw TAException("Not enough data");

  SMARes res;
  res.ma.resize(vSeries.size());

  TA_RetCode ta_ret = TA_MA(0, vSeries.size()-1, &vSeries[0], ma_period, TA_MAType_SMA, &res.begIdx, &res.nbElement, &res.ma[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("No output elements");

  return res;
}


TA::EMARes TA::EMA( vDouble vSeries, unsigned ema_period ) const throw(TAException)
{
  if( vSeries.size() < ema_period )
    throw TAException("Not enough data");

  EMARes res;
  res.ema.resize(vSeries.size());

  TA_RetCode ta_ret = TA_MA(0, vSeries.size()-1, &vSeries[0], ema_period, TA_MAType_EMA, &res.begIdx, &res.nbElement, &res.ema[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("No out elements");

  return res;
}


TA::RSIRes TA::RSI( vDouble vSeries, unsigned rsi_period ) const throw(TAException)
{
  if( vSeries.size() < rsi_period )
    throw TAException("Not enough data");

  RSIRes res;
  res.rsi.resize(vSeries.size());

  TA_RetCode ta_ret = TA_RSI(0, vSeries.size()-1, &vSeries[0], rsi_period, &res.begIdx, &res.nbElement, &res.rsi[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("No out elements");

  return res;
}


TA::ROCRes TA::ROC(vDouble vSeries, unsigned roc_period) const throw(TAException)
{
  if( vSeries.size() < roc_period )
    throw TAException("Not enough data");

  ROCRes res;
  res.roc.resize(vSeries.size());

  TA_RetCode ta_ret = TA_ROC(0, vSeries.size()-1, &vSeries[0], roc_period, &res.begIdx, &res.nbElement, &res.roc[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("No out elements");

  return res;
}


TA::ROCRRes TA::ROCR(vDouble vSeries, unsigned rocr_period ) const throw(TAException)
{
  if( vSeries.size() < rocr_period )
    throw TAException("Not enough data");

  ROCRRes res;
  res.rocr.resize(vSeries.size());

  TA_RetCode ta_ret = TA_ROCR(0, vSeries.size()-1, &vSeries[0], rocr_period, &res.begIdx, &res.nbElement, &res.rocr[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("No out elements");

  return res;
}


TA::ROCPRes TA::ROCP(vDouble vSeries, unsigned rocp_period ) const throw(TAException)
{
  if( vSeries.size() < rocp_period )
    throw TAException("Not enough data");

  ROCPRes res;
  res.rocp.resize(vSeries.size());

  TA_RetCode ta_ret = TA_ROCP(0, vSeries.size()-1, &vSeries[0], rocp_period, &res.begIdx, &res.nbElement, &res.rocp[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("No out elements");

  return res;
}


TA::STDDEVRes TA::STDDEV( vDouble vSeries, unsigned stddev_period, double sd ) const throw(TAException)
{
  if( vSeries.size() < stddev_period )
    throw TAException("Not enough data");

  STDDEVRes res;
  res.stddev.resize(vSeries.size());

  TA_RetCode ta_ret = TA_STDDEV(0, vSeries.size()-1, &vSeries[0], stddev_period, sd, &res.begIdx, &res.nbElement, &res.stddev[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("No out elements");

  return res;
}


TA::BBRes TA::BBANDS( vDouble vSeries, unsigned ma_period, double sd_up, double sd_down ) const throw(TAException)
{
  if( vSeries.size() < ma_period )
    throw TAException("Not enough data");

  BBRes res;
  res.lower_band.resize(vSeries.size());
  res.middle_band.resize(vSeries.size());
  res.upper_band.resize(vSeries.size());

  TA_RetCode ta_ret = TA_BBANDS(0, vSeries.size()-1, &vSeries[0], ma_period, sd_up, sd_down, TA_MAType_SMA,
                                &res.begIdx, &res.nbElement, &res.upper_band[0], &res.middle_band[0], &res.lower_band[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("No out elements");

  return res;
}


TA::MACDRes TA::MACD( vDouble vSeries, unsigned macd_fast_period, unsigned macd_slow_period, unsigned macd_signal_period ) throw(TAException)
{
  if( vSeries.size() < macd_slow_period ||
      vSeries.size() < macd_fast_period ||
      vSeries.size() < macd_signal_period )
    throw TAException("Not enough data");

  MACDRes res;
  res.macd.resize(vSeries.size());
  res.macd_hist.resize(vSeries.size());
  res.macd_signal.resize(vSeries.size());

  TA_RetCode ta_ret = TA_MACD(0, vSeries.size()-1, &vSeries[0], macd_fast_period, macd_slow_period, macd_signal_period,
                              &res.begIdx, &res.nbElement, &res.macd[0], &res.macd_signal[0], &res.macd_hist[0]);

  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("No out elements");

  return res;
}


TA::FACTORRes TA::FACTORS( vDouble vSeries, unsigned period ) const throw(TAException)
{
  if( (period + 1) > vSeries.size() )
    throw TAException("Not enough data");

  FACTORRes res;
  res.factors.resize(vSeries.size());

  double last_element = vSeries[0];
  for( unsigned i = period; i < vSeries.size(); i += period ) {
    res.factors.push_back(vSeries[i] / last_element);
    last_element = vSeries[i];
  }

  return res;
}


string TA::getError(TA_RetCode code) const
{
  TA_RetCodeInfo ta_retinfo;
  TA_SetRetCodeInfo(code, &ta_retinfo);
  return string(ta_retinfo.infoStr);
}

