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

#ifndef STRUCTUREDPOISSONGROUP_H_
#define STRUCTUREDPOISSONGROUP_H_

#include "auryn_definitions.h"
#include "System.h"
#include "SpikingGroup.h"
#include "PoissonGroup.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/exponential_distribution.hpp>

using namespace std;

/*! \brief A special Poisson generator that can hide a fixed number of spatio-temporal patterns in the spike data
 *
 * This class achieves inserting a fixed number of spatio temporal patterns that are injected at intervals that vary stochastically.
 * In detail this means, aftern an interval drawn from an exponential distribution the PoissonGroup is seeded to a fixed value from a
 * set determinded by the no_of_stimuli variable. The PoissonGroup is then left running for a time determined by stimulus_duration 
 * and then reseeded pseudo randomly from get_clock() - therefore a seed cannot repeat due to causality.
 */
class StructuredPoissonGroup : public PoissonGroup
{
private:
	static boost::mt19937 interval_gen;

	boost::exponential_distribution<> * dist;
	boost::variate_generator<boost::mt19937&, boost::exponential_distribution<> > * die;

	NeuronID no_of_stimuli;
	AurynFloat stimulus_duration;
	AurynFloat mean_isi;
	ofstream tiserfile;

	AurynTime next_event;
	bool stimulus_active;
	int current_stimulus;

	void init ( AurynFloat duration, AurynFloat mean_interval, NeuronID no , string outputfile );
	
public:
	StructuredPoissonGroup(NeuronID n, AurynFloat duration, AurynFloat interval, NeuronID stimuli = 1,  AurynDouble rate=5. ,
			string tiserfile = "stimulus.dat" );
	virtual ~StructuredPoissonGroup();
	virtual void evolve();
};

#endif /*STRUCTUREDPOISSONGROUP_H_*/