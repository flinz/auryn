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

#ifndef AUDITORYBEEPGROUP_H_
#define AUDITORYBEEPGROUP_H_

#include "auryn_definitions.h"
#include "System.h"
#include "SpikingGroup.h"
#include "PoissonGroup.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/exponential_distribution.hpp>

using namespace std;

/*! \brief A special Poisson generator that mimicks thalamo-cortical input to
 * auditory cortex layer 3/4.
 *
 * The object simulates a single pure tone stimulus at the beginning of every
 * stimulation_period during stimulus_duration. At the beginning of each
 * stimulus spikes occurr timelocked (every cell in the group spikes in exactly
 * the same time bin). Followed by a poisson firing with rate_on during the
 * stimulus_duration. Afterwards the group defaults back to normal poisson
 * firing with rate_off.
 */
class AuditoryBeepGroup : public PoissonGroup
{
private:
	AurynTime stimulus_duration;
	AurynTime stimulation_period;


	AurynTime next_event;
	bool stimulus_active;
	int current_stimulus;

	void init ( AurynFloat duration, AurynFloat mean_interval, AurynFloat rate );
	
public:

	/*! Describes the background firing rate in silence */
	AurynFloat rate_off;

	/*! Describes the mean firing rate during stimulus. 
	 * Always make sure this value is larger than zero
	 * otherwise there will be some weird behavior (and 
	 * this is not checked. */
	AurynFloat rate_on;

	AuditoryBeepGroup(NeuronID n, AurynFloat duration=50e-3, AurynFloat interval=1.0, AurynDouble rate=50.0 );
	virtual ~AuditoryBeepGroup();
	virtual void evolve();
};

#endif /*AUDITORYBEEPGROUP_H_*/
