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

#ifndef STIMULUSGROUP_H_

#define STIMULUSGROUP_H_

#include "auryn_definitions.h"
#include "System.h"
#include "SpikingGroup.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#define BASERATE 1.0
#define SOFTSTARTTIME 0.1
#define STIMULUSGROUP_LOAD_MULTIPLIER 0.1

using namespace std;


/*! \brief Provides a poisson stimulus at random intervals in one or more
 *         predefined subsets of the group that are read from a file. */
class StimulusGroup : public SpikingGroup
{
private:
	AurynTime * clk;
	AurynTime * ttl;
	vector<type_pattern> stimuli;
	AurynFloat * activity;
	ofstream tiserfile;
	AurynFloat base_rate;

	int off_pattern;

	/*! pseudo random number generators */
	static boost::mt19937 poisson_gen; 

	/*! generates info for what stimulus is active. Is supposed to give the same result on all nodes (hence same seed required) */
	static boost::mt19937 order_gen; 
	static boost::uniform_01<boost::mt19937> order_die; 

	/*! Stimulus order */
	StimulusGroupModeType stimulus_order ;

	/*! stimulus probabilities */
	vector<double> probabilities ;

	/*! current stimulus index */
	unsigned int cur_stim_index ;
	bool stimulus_active;

	/*! next stimulus time requiring change in rates */
	AurynTime next_action_time ;

	/*! Standard initialization */
	void init(string filename, StimulusGroupModeType stimulusmode, string outputfile, AurynFloat baserate);
	/*! Draw all Time-To-Live (ttls) typically after changing the any of the activiteis */
	void redraw();

	/*! Makes onset of stimulus soft */
	void redraw_softstart();

	/*! write current stimulus to timeseriesfile */
	void write_sequence_file(AurynDouble time);

	/*! Sets the activity for a given unit on the local rank. Activity determines the freq as baserate*activity */
	void set_activity( NeuronID i, AurynFloat val=0.0 );

	/*! allow silence/background activity periods */
	AurynFloat mean_off_period ;

	/*! mean presentation time  */
	AurynFloat mean_on_period ;
	
public:
	/*! This is by how much the pattern gamma value is multiplied. The resulting value gives the x-times baseline activation */
	AurynFloat scale;

	/*! Ignore gamma value */
	bool binary_patterns;

	/*! Determines if the Group is active or bypassed upon evolution. */
	bool active;

	/*! Determines if the Group is using random activation intervals */
	bool randomintervals;

	/*! Default constructor */
	StimulusGroup(NeuronID n, string filename, string outputfile = "", StimulusGroupModeType stimulusmode=RANDOM, AurynFloat baserate=0.0 );


	virtual ~StimulusGroup();
	/*! Standard virtual evolve function */
	virtual void evolve();
	/*! Sets the baserate that is the rate at 1 activity */
	void set_baserate(AurynFloat baserate);
	void set_maxrate(AurynFloat baserate); // TODO remove deprecated
	/*! Sets sets the activity of all units */
	void set_all( AurynFloat val=0.0 );
	/*! Gets the activity of unit i */
	AurynFloat get_activity(NeuronID i);

	/*! Loads stimulus patterns from a designated file given */
	void load_patterns( string filename );

	/*! Set mean quiet interval between consecutive stimuli */
	void set_mean_off_period(AurynFloat period);

	/*! Set mean on period */
	void set_mean_on_period(AurynFloat period);

	void set_pattern_activity( unsigned int i );
	void set_pattern_activity( unsigned int i, AurynFloat setval );
	void set_active_pattern( unsigned int i );

	/*! Select one pattern that is active during the off period.
	 *  If set there is always a pattern active if set.
	 *  Set to negative value to disable. */
	void set_off_pattern( int i );

	void set_next_action_time(double time);

	/*! Setter for pattern probability distribution */
	void set_distribution ( vector<double> probs );
	/*! Getter for pattern probability distribution */
	vector<double> get_distribution ( );
	/*! Getter for pattern i of the probability distribution */
	double get_distribution ( int i );
	/*! Initialized distribution to be flat */
	void flat_distribution( );
	/*! Normalizes the distribution */
	void normalize_distribution( );

	vector<type_pattern> * get_patterns();

};

#endif /*STIMULUSGROUP_H_*/
