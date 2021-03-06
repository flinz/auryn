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

#include "EulerTrace.h"

void EulerTrace::init(NeuronID n, AurynFloat timeconstant)
{
	size = n;
	set_timeconstant(timeconstant);
	state = gsl_vector_float_alloc ( calculate_vector_size(size) ); 
	setall(0.);
	target_ptr = NULL;
}

void EulerTrace::free()
{
	gsl_vector_float_free (state);

	if ( target_ptr != NULL ) {
		gsl_vector_float_free (update);
	}
}

EulerTrace::EulerTrace(NeuronID n, AurynFloat timeconstant)
{
	init(n,timeconstant);
}

EulerTrace::~EulerTrace()
{
	free();
}

void EulerTrace::set_timeconstant(AurynFloat timeconstant)
{
	tau = timeconstant;
	scale_const = exp(-dt/tau);
}

void EulerTrace::set(NeuronID i , AurynFloat value)
{
   gsl_vector_float_set (state, i, value);
}

void EulerTrace::setall(AurynFloat value)
{
	for (NeuronID i = 0 ; i < size ; ++i )
		set(i,value);
}

void EulerTrace::add(gsl_vector_float * values)
{
   gsl_vector_float_add ( state, values );
}

void EulerTrace::add(NeuronID i, AurynFloat value)
{
   // gsl_vector_float_set (state, i, gsl_vector_float_get (state, i) + value);
   state->data[i] += value;
}

void EulerTrace::set_target( gsl_vector_float * target )
{
	if ( target != NULL ) {
		update = gsl_vector_float_alloc (size); 
	}
	target_ptr = target ;
}

void EulerTrace::set_target( EulerTrace * target )
{
	set_target( target->state );
}

void EulerTrace::evolve()
{
    auryn_vector_float_scale(scale_const,state); // seems to be faster
	// auryn_vector_float_mul_constant(state,scale_const);
}

void EulerTrace::follow()
{ 
	gsl_blas_scopy( state, update );
	gsl_blas_saxpy( -1., target_ptr, update );
	gsl_blas_saxpy( -dt/tau, update, state );
}

gsl_vector_float * EulerTrace::get_state_ptr()
{
	return state;
}

AurynFloat EulerTrace::get_tau()
{
	return tau;
}

void EulerTrace::inc(NeuronID i)
{
   // gsl_vector_float_set (state, i, gsl_vector_float_get (state, i)+1);
   state->data[i]++;
}

AurynFloat EulerTrace::get(NeuronID i)
{
	return gsl_vector_float_get (state, i);
}

AurynFloat EulerTrace::normalized_get(NeuronID i)
{
	return gsl_vector_float_get (state, i) / tau ;
}


void EulerTrace::clip(AurynState value)
{
	auryn_vector_float_clip( state, 0.0, value);
}
