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

#include "AIF2Group.h"


AIF2Group::AIF2Group( NeuronID size, AurynFloat load, NeuronID total ) : AIFGroup(size,load,total)
{
	// sys->register_spiking_group(this); // already registered in AIFGroup
	if ( evolve_locally() ) init();
}

void AIF2Group::calculate_scale_constants()
{
	AIFGroup::calculate_scale_constants();
	scale_adapt2 = exp(-dt/tau_adapt2);
}

void AIF2Group::init()
{
	tau_adapt2 = 20.0;
	dg_adapt2  = 0.002;

	calculate_scale_constants();
	g_adapt2 = get_state_vector ("g_adapt2");

	clear();
}

void AIF2Group::clear()
{
	AIFGroup::clear();
	for (NeuronID i = 0; i < get_rank_size(); i++) {
	   gsl_vector_float_set (g_adapt2, i, 0.);
	 }
}


void AIF2Group::random_adapt(AurynState mean, AurynState sigma)
{
	boost::mt19937 ng_gen(42); // produces same series every time 
	boost::normal_distribution<> dist((double)mean, (double)sigma);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > die(ng_gen, dist);
	AurynState rv;

	for ( AurynLong i = 0 ; i<get_rank_size() ; ++i ) {
		rv = die();
		if ( rv>0 ) 
			set_val (g_adapt1, i, rv ); 
		rv = die();
		if ( rv>0 ) 
			set_val (g_adapt2, i, rv ); 
	}

	init_state();
}

void AIF2Group::free()
{
}

AIF2Group::~AIF2Group()
{
	if ( evolve_locally() ) free();
}

void AIF2Group::integrate_linear_nmda_synapses()
{
	// decay of ampa and gaba channel, i.e. multiply by exp(-dt/tau)
    auryn_vector_float_scale(scale_ampa,g_ampa);
    auryn_vector_float_scale(scale_gaba,g_gaba);
    auryn_vector_float_scale(scale_adapt1,g_adapt1);
    auryn_vector_float_scale(scale_adapt2,g_adapt2);

    // compute dg_nmda = (g_ampa-g_nmda)*dt/tau_nmda and add to g_nmda
	AurynFloat mul_nmda = dt/tau_nmda;
    auryn_vector_float_saxpy(mul_nmda,g_ampa,g_nmda);
	auryn_vector_float_saxpy(-mul_nmda,g_nmda,g_nmda);

    // excitatory
    gsl_blas_scopy(g_ampa,t_exc);
    auryn_vector_float_scale(-A_ampa,t_exc);
    auryn_vector_float_saxpy(-A_nmda,g_nmda,t_exc);
    auryn_vector_float_mul(t_exc,mem);
    
    // inhibitory
    gsl_blas_scopy(g_gaba,t_leak); // using t_leak as temp here
    auryn_vector_float_saxpy(1,g_adapt1,t_leak);
    auryn_vector_float_saxpy(1,g_adapt2,t_leak);
    gsl_blas_scopy(mem,t_inh);
    auryn_vector_float_add_constant(t_inh,-e_rev);
    auryn_vector_float_mul(t_inh,t_leak);
}


void AIF2Group::check_thresholds()
{
	auryn_vector_float_clip( mem, e_rev );

	float * thr_ptr = thr->data;
	for ( float * i = mem->data ; i != mem->data+get_rank_size() ; ++i ) { // it's important to use rank_size here otherwise there might be spikes from units that do not exist
    	if ( *i > ( thr_rest + *thr_ptr ) ) {
			NeuronID unit = i-mem->data;
			push_spike(unit);
		    set_val (mem, unit, e_rest); // reset
	        set_val (thr, unit, dthr); //refractory
			add_val (g_adapt1, unit, dg_adapt1);
			add_val (g_adapt2, unit, dg_adapt2);
		} 
		thr_ptr++;
	}

}

void AIF2Group::evolve()
{
	integrate_linear_nmda_synapses();
	integrate_membrane();
	check_thresholds();
}



