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

#include "PoissonGroup.h"

boost::mt19937 PoissonGroup::gen = boost::mt19937(); 

void PoissonGroup::init(AurynDouble  rate)
{
	sys->register_spiking_group(this);
	if ( evolve_locally() ) {
		lambda = rate;

		dist = new boost::uniform_01<> ();
		die  = new boost::variate_generator<boost::mt19937&, boost::uniform_01<> > ( gen, *dist );
		seed(communicator->rank()); // seeding problem
		x = 0;

		stringstream oss;
		oss << "PoissonGroup:: Seeding with " << communicator->rank();
		logger->msg(oss.str(),NOTIFICATION);
	}
}

PoissonGroup::PoissonGroup(NeuronID n, AurynDouble  rate ) : SpikingGroup( n , POISSON_LOAD_MULTIPLIER*rate ) 
{
	init(rate);
}

PoissonGroup::~PoissonGroup()
{
	if ( evolve_locally() ) {
		delete dist;
		delete die;
	}
}

void PoissonGroup::set_rate(AurynDouble  rate)
{
	lambda = rate;
    if (evolve_locally() ) {
      AurynDouble r = -log((*die)()+1e-20)/lambda;
      x = (NeuronID)(r/dt); 
    }
}

AurynDouble  PoissonGroup::get_rate()
{
	return lambda;
}


void PoissonGroup::evolve()
{
	while ( x < get_rank_size() ) {
		push_spike ( x );
		AurynDouble r = -log((*die)()+1e-20)/lambda;
		x += 1+(NeuronID)(r/dt); 
		// beware one induces systematic error that becomes substantial at high rates, but keeps neuron from spiking twice per time-step
	}
	x -= get_rank_size();
}

void PoissonGroup::seed(int s)
{
		gen.seed(s); 
}

