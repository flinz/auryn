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

#include "NeuronGroup.h"

NeuronGroup::NeuronGroup(NeuronID n, double loadmultiplier, NeuronID total ) : SpikingGroup(n, loadmultiplier, total )
{
	if ( evolve_locally() ) init();
}

void NeuronGroup::init()
{  		group_name = "NeuronGroup";

		// stringstream oss;
		// oss << description_string << " init";
		// logger->msg(oss.str(),DEBUG);

		mem = get_state_vector("mem");
		thr = get_state_vector("thr");
		g_ampa = get_state_vector("g_ampa");
		g_gaba = get_state_vector("g_gaba");
		g_nmda = get_state_vector("g_nmda");

#ifndef CODE_ALIGNED_SSE_INSTRUCTIONS
		// checking via default if those arrays are aligned
		if ( auryn_AlignOffset( mem->size, mem->data, sizeof(float), 16) 
				|| auryn_AlignOffset( thr->size, thr->data, sizeof(float), 16) 
				|| auryn_AlignOffset( g_ampa->size, g_ampa->data, sizeof(float), 16) 
				|| auryn_AlignOffset( g_nmda->size, g_nmda->data, sizeof(float), 16) 
				|| auryn_AlignOffset( g_gaba->size, g_gaba->data, sizeof(float), 16) ) 
			throw AurynMemoryAlignmentException();
#endif
}


void NeuronGroup::free()
{
}


NeuronGroup::~NeuronGroup()
{
	if ( evolve_locally() ) free();
}

void NeuronGroup::print_val(gsl_vector_float * vec, NeuronID i, const char * name)
{
           printf ("%s%d= %g\n",name, i, gsl_vector_float_get (vec, i));
}

void NeuronGroup::print_vec(gsl_vector_float * vec, const char * name)
{
	for (NeuronID i = 0; i < get_rank_size(); i++)
         {
			 print_val(vec,i,name);
         }
}

void NeuronGroup::print_state(NeuronID id)
{
           printf ("%g %g %g %g\n",gsl_vector_float_get (mem, id), gsl_vector_float_get (g_ampa, id), gsl_vector_float_get (g_gaba, id) ,gsl_vector_float_get (g_nmda, id));
}

AurynState NeuronGroup::get_mem(NeuronID i)
{
	return get_val(mem,i);
}

gsl_vector_float * NeuronGroup::get_mem_ptr()
{
	return mem;
}

void NeuronGroup::set_mem(NeuronID i, AurynState val)
{
	set_val(mem,i,val);
}


AurynState NeuronGroup::get_ampa(NeuronID i)
{
	return get_val(g_ampa,i);
}

gsl_vector_float * NeuronGroup::get_ampa_ptr()
{
	return g_ampa;
}

AurynState NeuronGroup::get_gaba(NeuronID i)
{
	return get_val(g_gaba,i);
}

gsl_vector_float * NeuronGroup::get_gaba_ptr()
{
	return g_gaba;
}

AurynState NeuronGroup::get_nmda(NeuronID i)
{
	return get_val(g_nmda,i);
}

gsl_vector_float * NeuronGroup::get_nmda_ptr()
{
	return g_nmda;
}

void NeuronGroup::set_ampa(NeuronID i, AurynState val)
{
	set_val(g_ampa,i,val);
}

void NeuronGroup::set_gaba(NeuronID i, AurynState val)
{
	set_val(g_gaba,i,val);
}

void NeuronGroup::set_nmda(NeuronID i, AurynState val)
{
	set_val(g_nmda,i,val);
}

void NeuronGroup::random_mem(AurynState mean, AurynState sigma)
{
	boost::mt19937 ng_gen(42+communicator->rank()); // produces same series every time 
	boost::normal_distribution<> dist((double)mean, (double)sigma);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > die(ng_gen, dist);
	AurynState rv;

	for ( AurynLong i = 0 ; i<get_rank_size() ; ++i ) {
		rv = die();
		set_mem(i,rv);
	}

	init_state();
}

void NeuronGroup::random_uniform_mem(AurynState lo, AurynState hi)
{
	boost::mt19937 ng_gen(42+communicator->rank()); // produces same series every time 
	boost::uniform_01<boost::mt19937> die = boost::uniform_01<boost::mt19937> (ng_gen);
	AurynState rv;

	for ( AurynLong i = 0 ; i<get_rank_size() ; ++i ) {
		rv = lo+die()*(hi-lo);
		set_mem(i,rv);
	}

	init_state();
}

void NeuronGroup::random_nmda(AurynState mean, AurynState sigma)
{
	boost::mt19937 ng_gen(53+communicator->rank()); // produces same series every time 
	boost::normal_distribution<> dist((double)mean, (double)sigma);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > die(ng_gen, dist);
	AurynState rv;

	for ( AurynLong i = 0 ; i<get_rank_size() ; ++i ) {
		rv = die();
		set_nmda(i,rv);
	}

	init_state();
}

void NeuronGroup::print_mem()
{
	print_vec(mem,"mem");
}

void NeuronGroup::print_ampa()
{
	print_vec(g_ampa,"g_ampa");
}

void NeuronGroup::print_gaba()
{
	print_vec(g_gaba,"g_gaba");
}

void NeuronGroup::print_nmda()
{
	print_vec(g_nmda,"g_nmda");
}

void NeuronGroup::safe_tadd(NeuronID id, AurynWeight amount, TransmitterType t)
{
	if (localrank(id))
		tadd(id, amount, t);
}

void NeuronGroup::init_state()
{

}






void NeuronGroup::set_val(gsl_vector_float * vec, NeuronID i, AurynState val)
{
    gsl_vector_float_set (vec, i, val);
}

void NeuronGroup::add_val(gsl_vector_float * vec, NeuronID i, AurynState val)
{
	// gsl_vector_float_set(vec,i,gsl_vector_float_get(vec,i)+val);
	vec->data[i] += val;
}

void NeuronGroup::clip_val(gsl_vector_float * vec, NeuronID i, AurynState max)
{
	if ( gsl_vector_float_get(vec,i) > max)
		gsl_vector_float_set(vec,i,max);
}

AurynState NeuronGroup::get_val(gsl_vector_float * vec, NeuronID i)
{
	return gsl_vector_float_get (vec, i);
}

void NeuronGroup::tadd(NeuronID id, AurynWeight amount, TransmitterType t)
{
	NeuronID localid = global2rank(id);
	if (t==GLUT) {
		add_val(g_ampa,localid,amount);
	} else {
		add_val(g_gaba,localid,amount);
	}
}

