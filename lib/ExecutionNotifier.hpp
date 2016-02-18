/*
* Copyright (C) 2007,2008 Alberto Giannetti
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

#ifndef _EXECUTIONNOTIFIER_HPP_
#define _EXECUTIONNOTIFIER_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <list>

// Hudson
#include "Execution.hpp"
#include "ExecutionPtr.hpp"

class ExecutionObserver;


class ExecutionNotifier
{
public:
  virtual ~ExecutionNotifier(void) { }

  virtual void attach(ExecutionObserver* pObserver);
  virtual void detach(ExecutionObserver* pObserver);

protected:
  ExecutionNotifier(void) { }

  virtual void notify(const ExecutionPtr pExe);

protected:
  typedef std::list<ExecutionObserver*> LEO;
  LEO _lExecObs;
};

#endif // _EXECUTIONNOTIFIER_HPP_
