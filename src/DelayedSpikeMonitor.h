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

#ifndef DELAYEDSPIKEMONITOR_H_
#define DELAYEDSPIKEMONITOR_H_

#include "auryn_definitions.h"
#include "SpikingGroup.h"
#include "Monitor.h"
#include "System.h"
#include <fstream>

using namespace std;

/*! \brief SpikeMonitor that reads the delayed spikes as they are
 *         received by a postsynaptic neuron.
 *
 * This monitor has mainly testing pruposes since it records all
 * the spikes on each node (which effectively multiplies spikes.
 */
class DelayedSpikeMonitor : Monitor
{
private:
    NeuronID n_from;
    NeuronID n_to;
	SpikeContainer::const_iterator it;
	SpikingGroup * src;
	NeuronID offset;
	void init(SpikingGroup * source, string filename, NeuronID from, NeuronID to);
	void free();
	
public:
	DelayedSpikeMonitor(SpikingGroup * source, string filename);
	DelayedSpikeMonitor(SpikingGroup * source, string filename, NeuronID to);
	DelayedSpikeMonitor(SpikingGroup * source, string filename, NeuronID from, NeuronID to);
	void set_offset(NeuronID of);
	virtual ~DelayedSpikeMonitor();
	void propagate();
};

#endif /*DELAYEDSPIKEMONITOR_H_*/
