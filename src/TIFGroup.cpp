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

#include "TIFGroup.h"

TIFGroup::TIFGroup(NeuronID size) : NeuronGroup(size)
{
	sys->register_spiking_group(this);
	if ( evolve_locally() ) init();
}

void TIFGroup::calculate_scale_constants()
{
	scale_mem  = dt/tau_mem;
	scale_ampa = exp(-dt/tau_ampa);
	scale_gaba = exp(-dt/tau_gaba);
}

void TIFGroup::init()
{
	e_rest = -60e-3;
	e_rev = -80e-3;
	thr = -50e-3;
	tau_ampa = 5e-3;
	tau_gaba = 10e-3;
	tau_mem = 20e-3;
	refractory_time = (unsigned short) (5.e-3/dt);

	calculate_scale_constants();
	
	ref = gsl_vector_ushort_alloc (get_vector_size()); 
	bg_current = get_state_vector("bg_current");

	t_g_ampa = gsl_vector_float_ptr ( g_ampa , 0 ); 
	t_g_gaba = gsl_vector_float_ptr ( g_gaba , 0 ); 
	t_bg_cur = gsl_vector_float_ptr ( bg_current , 0 ); 
	t_mem = gsl_vector_float_ptr ( mem , 0 ); 
	t_ref = gsl_vector_ushort_ptr ( ref , 0 ); 

	clear();

}

void TIFGroup::clear()
{
	clear_spikes();
	for (NeuronID i = 0; i < get_rank_size(); i++) {
	   gsl_vector_float_set (mem, i, e_rest);
	   gsl_vector_ushort_set (ref, i, 0);
	   gsl_vector_float_set (g_ampa, i, 0.);
	   gsl_vector_float_set (g_gaba, i, 0.);
	   gsl_vector_float_set (bg_current, i, 0.);
	}
}


TIFGroup::~TIFGroup()
{
	if ( !evolve_locally() ) return;

	gsl_vector_ushort_free (ref);
}


void TIFGroup::evolve()
{
	for (NeuronID i = 0 ; i < get_rank_size() ; ++i ) {
    	if (t_ref[i]==0) {
			const AurynFloat dg_mem = ( (e_rest-t_mem[i]) 
					- t_g_ampa[i] * t_mem[i]
					- t_g_gaba[i] * (t_mem[i]-e_rev)
					+ t_bg_cur[i] );
			t_mem[i] += dg_mem*scale_mem;

			if (t_mem[i]>thr) {
				push_spike(i);
				t_mem[i] = e_rest ;
				t_ref[i] += refractory_time ;
			} 
		} else {
			t_ref[i]-- ;
			t_mem[i] = e_rest ;
		}

	}

    auryn_vector_float_scale(scale_ampa,g_ampa);
    auryn_vector_float_scale(scale_gaba,g_gaba);
}

void TIFGroup::set_bg_current(NeuronID i, AurynFloat current) {
	if ( localrank(i) )
		gsl_vector_float_set ( bg_current , global2rank(i) , current ) ;
}

void TIFGroup::set_tau_mem(AurynFloat taum)
{
	tau_mem = taum;
	calculate_scale_constants();
}

AurynFloat TIFGroup::get_bg_current(NeuronID i) {
	if ( localrank(i) )
		return gsl_vector_float_get ( bg_current , global2rank(i) ) ;
	else 
		return 0;
}

string TIFGroup::get_output_line(NeuronID i)
{
	stringstream oss;
	oss << get_mem(i) << " " << get_ampa(i) << " " << get_gaba(i) << " " << gsl_vector_ushort_get (ref, i) << "\n";
	return oss.str();
}

void TIFGroup::load_input_line(NeuronID i, const char * buf)
{
		float vmem,vampa,vgaba;
		NeuronID vref;
		sscanf (buf,"%f %f %f %u",&vmem,&vampa,&vgaba,&vref);
		if ( localrank(i) ) {
			NeuronID trans = global2rank(i);
			set_mem(trans,vmem);
			set_ampa(trans,vampa);
			set_gaba(trans,vgaba);
			gsl_vector_ushort_set (ref, trans, vref);
		}
}

void TIFGroup::set_tau_ampa(AurynFloat taum)
{
	tau_ampa = taum;
	calculate_scale_constants();
}

AurynFloat TIFGroup::get_tau_ampa()
{
	return tau_ampa;
}

void TIFGroup::set_tau_gaba(AurynFloat taum)
{
	tau_gaba = taum;
	calculate_scale_constants();
}

AurynFloat TIFGroup::get_tau_gaba()
{
	return tau_gaba;
}

