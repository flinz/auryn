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

#ifndef AIF2GROUP_H_
#define AIF2GROUP_H_

#include "auryn_definitions.h"
#include "AIFGroup.h"
#include "System.h"
#include <gsl/gsl_blas.h>

class AIF2Group : public AIFGroup
{
private:
	AurynFloat scale_adapt2;
	AurynFloat tau_adapt2;

	void init();
	void free();

protected:
	gsl_vector_float * g_adapt2;

	void calculate_scale_constants();
	void integrate_linear_nmda_synapses();
	void check_thresholds();

public:
	AIF2Group( NeuronID size, AurynFloat load = 1.0, NeuronID total = 0 );
	virtual ~AIF2Group();

	void random_adapt(AurynState mean, AurynState sigma);

	AurynFloat dg_adapt2;

	void clear();
	virtual void evolve();
};

#endif /*AIF2GROUP_H_*/

