/*
* Copyright (C) 2007,2008, A. Giannetti
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

// Hudson
#include "ExecutionNotifier.hpp"
#include "ExecutionObserver.hpp"


void ExecutionNotifier::attach(ExecutionObserver* pObserver)
{
  _lExecObs.push_back(pObserver);
}


void ExecutionNotifier::detach(ExecutionObserver* pObserver)
{
  _lExecObs.remove(pObserver);
}


void ExecutionNotifier::notify(const ExecutionPtr pExe)
{
  for( LEO::iterator iter(_lExecObs.begin()); iter != _lExecObs.end(); ++iter )
    (*iter)->update(pExe);
}
