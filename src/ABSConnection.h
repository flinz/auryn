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

#ifndef ABSCONNECTION_H_
#define ABSCONNECTION_H_

#define ABS_VOLTAGE_CURVE_SIZE 100
#define ABS_VOLTAGE_CURVE_MIN -80e-3
#define ABS_VOLTAGE_CURVE_MAX -40e-3

#include "auryn_definitions.h"
#include "DuplexConnection.h"
#include "EulerTrace.h"

using namespace std;


class ABSConnection : public DuplexConnection
{

protected:
	AurynFloat tau_post;
	EulerTrace * tr_post;

	inline AurynWeight dw_fwd(NeuronID post);
	inline AurynWeight dw_bkw(NeuronID pre);
	virtual AurynFloat etamod(NeuronID post);

protected:

	void propagate_forward();
	void propagate_backward();


public:
	AurynFloat * voltage_curve_post;

	bool stdp_active;

	ABSConnection(SpikingGroup * source, NeuronGroup * destination, 
			const char * filename, 
			AurynWeight maxweight=1. , TransmitterType transmitter=GLUT);

	ABSConnection(SpikingGroup * source, NeuronGroup * destination, 
			AurynWeight weight, AurynFloat sparseness=0.05,
			AurynWeight maxweight=1. , TransmitterType transmitter=GLUT, string name="ABSConnection");
	virtual ~ABSConnection();
	void init(AurynWeight maxw);
	void free();

	void set_default_curve(double fp_low=-60e-3, double fp_middle=-55e-3, double fp_high=-50e-3, double scale=100);
	void load_curve_from_file( const char * filename , double scale = 1. );

	virtual void propagate();

};

#endif /*ABSCONNECTION_H_*/