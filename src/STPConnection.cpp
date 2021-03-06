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

#include "STPConnection.h"

void STPConnection::init() 
{
	if ( src->get_rank_size() > 0 ) {
		// init of STP stuff
		tau_d = 0.2;
		tau_f = 1.0;
		Urest = 0.3;
		Ujump = 0.01;
		state_x = gsl_vector_float_alloc( src->get_rank_size() );
		state_u = gsl_vector_float_alloc( src->get_rank_size() );
		state_temp = gsl_vector_float_alloc( src->get_rank_size() );
		for (NeuronID i = 0; i < src->get_rank_size() ; i++)
		{
			   gsl_vector_float_set (state_x, i, 1 ); // TODO
			   gsl_vector_float_set (state_u, i, Ujump );
		}

	}

	// registering the right amount of spike attributes
	// this line is very important finding bugs due to 
	// this being wrong or missing is hard 
	src->set_num_spike_attributes(1);

}


STPConnection::STPConnection(const char * filename) 
: SparseConnection(filename)
{
	if ( dst->get_post_size() > 0 ) 
		init();
}

STPConnection::STPConnection(SpikingGroup * source, NeuronGroup * destination, 
		TransmitterType transmitter) 
: SparseConnection(source, destination, transmitter)
{
}

STPConnection::STPConnection(SpikingGroup * source, NeuronGroup * destination, 
		const char * filename , 
		TransmitterType transmitter) 
: SparseConnection(source, destination, filename, transmitter)
{
	if ( dst->get_post_size() > 0 ) 
		init();
}


STPConnection::STPConnection(NeuronID rows, NeuronID cols) 
: SparseConnection(rows,cols)
{
	init();
}

STPConnection::STPConnection( SpikingGroup * source, NeuronGroup * destination, 
		AurynWeight weight, AurynFloat sparseness, 
		TransmitterType transmitter, string name) 
: SparseConnection(source,destination,weight,sparseness,transmitter, name)
{
	if ( dst->get_post_size() > 0 ) 
		init();
}

void STPConnection::free()
{
	if ( src->get_rank_size() > 0 ) {
		gsl_vector_float_free (state_x);
		gsl_vector_float_free (state_u);
		gsl_vector_float_free (state_temp);
	}
}



STPConnection::~STPConnection()
{
	if ( dst->get_post_size() > 0 ) 
		free();
}

void STPConnection::push_attributes()
{
	SpikeContainer * spikes = src->get_spikes_immediate();
	for (SpikeContainer::const_iterator spike = spikes->begin() ;
			spike != spikes->end() ; ++spike ) {
		// dynamics 
		NeuronID spk = src->global2rank(*spike);
		double x = gsl_vector_float_get( state_x, spk );
		double u = gsl_vector_float_get( state_u, spk );
		gsl_vector_float_set( state_x, spk, x-u*x );
		gsl_vector_float_set( state_u, spk, u+Ujump*(1-u) );

		// TODO spike translation or introduce local_spikes function in SpikingGroup and implement this there ... (better option)
		src->push_attribute( x*u ); 
	}
}

void STPConnection::evolve()
{
	// dynamics of x
	gsl_vector_float_set_all( state_temp, 1);
	gsl_blas_saxpy(-1,state_x,state_temp);
	gsl_blas_saxpy(dt/tau_d,state_temp,state_x);

	// dynamics of u
	gsl_vector_float_set_all( state_temp, Ujump);
	gsl_blas_saxpy(-1,state_u,state_temp);
	gsl_blas_saxpy(dt/tau_f,state_temp,state_u);

	// double x = gsl_vector_float_get( state_x, 0 );
	// double u = gsl_vector_float_get( state_u, 0 );
	// cout << setprecision(5) << x << " " << u << " " << x*u << endl;
}

void STPConnection::propagate()
{
	if ( src->evolve_locally()) {
		push_attributes(); // stuffs all attributes into the SpikeDelays for sync
	}

	if ( dst->evolve_locally() ) { // necessary 

		if (src->get_spikes()->size()>0) {
			NeuronID * ind = w->get_row_begin(0); // first element of index array
			AurynWeight * data = w->get_data_begin();
			AttributeContainer::const_iterator attr = src->get_attributes()->begin();
			SpikeContainer::const_iterator spikes_end = src->get_spikes()->end();
			for (SpikeContainer::const_iterator spike = src->get_spikes()->begin() ;
					spike != spikes_end ; ++spike ) {
				for (NeuronID * c = w->get_row_begin(*spike) ; c != w->get_row_end(*spike) ; ++c ) {
					AurynWeight value = data[c-ind] * *attr; 
					transmit( *c , value );
				}
				++attr;
			}
		}
	}
}

