/* 
* Copyright 2014 Friedemann Zenke
*
* This file is part of Auryn, a simulation package for plastic
* spiking neural networks.
* 
* Auryn is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Auryn is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with Auryn.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WEIGHTSUMMONITOR_H_
#define WEIGHTSUMMONITOR_H_

#include "auryn_definitions.h"
#include "Monitor.h"
#include "System.h"
#include "Connection.h"
#include <fstream>
#include <iomanip>

using namespace std;

/*! \brief Records sum and standard deviation of a weight matrix in predefined
 *         intervals.
 */
class WeightSumMonitor : protected Monitor
{
protected:
	Connection * src;
	AurynTime ssize;
	NeuronID data_size_limit;
	void init(Connection * source, string filename, AurynTime stepsize);
	
public:
	WeightSumMonitor(Connection * source, string filename, AurynTime stepsize=10000);
	virtual ~WeightSumMonitor();
	void propagate();
};

#endif /*WEIGHTSUMMONITOR_H_*/
