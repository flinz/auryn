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

#ifndef TRIPLETDECAYCONNECTION_H_
#define TRIPLETDECAYCONNECTION_H_

#include "auryn_definitions.h"
#include "TripletConnection.h"
#include "EulerTrace.h"

#define TRIPLETDECAYCONNECTION_EULERUPGRADE_STEP 0.999

using namespace std;


class TripletDecayConnection : public TripletConnection
{

private:
	AurynTime decay_timestep;
	AurynFloat tau_decay;
	AurynFloat mul_decay;
	AurynWeight w_rest;
	AurynInt decay_count;

public:
	TripletDecayConnection(SpikingGroup * source, NeuronGroup * destination, TransmitterType transmitter);

	TripletDecayConnection(SpikingGroup * source, NeuronGroup * destination, 
			const char * filename, 
			AurynFloat tau_hom=10, 
			AurynFloat eta=1, AurynFloat decay = 1e-3,
			AurynFloat kappa=3., AurynWeight wrest=0., AurynWeight maxweight=1. , 
			TransmitterType transmitter=GLUT);

	TripletDecayConnection(SpikingGroup * source, NeuronGroup * destination, 
			AurynWeight weight, AurynFloat sparseness=0.05, 
			AurynFloat tau_hom=10, 
			AurynFloat eta=1, AurynFloat decay=1e-3,
			AurynFloat kappa=3., AurynWeight wrest=0., AurynWeight maxweight=1. , 
			TransmitterType transmitter=GLUT);

	virtual ~TripletDecayConnection();
	void init(AurynFloat decay, AurynWeight wrest);
	void free();

	virtual void propagate();

};

#endif /*TRIPLETDECAYCONNECTION_H_*/
