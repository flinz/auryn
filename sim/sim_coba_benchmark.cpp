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

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>

#include <boost/program_options.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi.hpp>

#include "System.h"
#include "IFGroup.h"
#include "TIFGroup.h"
#include "PoissonGroup.h"
#include "SparseConnection.h"
#include "SymmetricSTDPConnection.h"
#include "WeightSumMonitor.h"
#include "SpikeMonitor.h"
#include "StateMonitor.h"
#include "RateChecker.h"

using namespace std;

namespace po = boost::program_options;
namespace mpi = boost::mpi;

int main(int ac,char *av[]) {
	string dir = "/tmp";

	string fwmat_ee = "";
	string fwmat_ei = "";
	string fwmat_ie = "";
	string fwmat_ii = "";

	stringstream oss;
	string strbuf ;
	string msg;

	double w = 0.4;
	double wi = 5.1;



	double sparseness = 0.02;
	double simtime = 20.;

	NeuronID ne = 3200;
	NeuronID ni = 800;

	bool prime = false;
	bool fast = false;

	int errcode = 0;


    try {

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("simtime", po::value<double>(), "simulation time")
            ("prime", "switch input modalities")
            ("fast", "turns off most monitoring to reduce IO")
            ("dir", po::value<string>(), "load/save directory")
            ("fee", po::value<string>(), "file with EE connections")
            ("fei", po::value<string>(), "file with EI connections")
            ("fie", po::value<string>(), "file with IE connections")
            ("fii", po::value<string>(), "file with II connections")
        ;

        po::variables_map vm;        
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);    

        if (vm.count("help")) {
            cout << desc << "\n";
            return 1;
        }

        if (vm.count("simtime")) {
			simtime = vm["simtime"].as<double>();
        } 

        if (vm.count("prime")) {
			prime = true;
        } 

        if (vm.count("fast")) {
			fast = true;
        } 

        if (vm.count("dir")) {
			dir = vm["dir"].as<string>();
        } 

        if (vm.count("fee")) {
			fwmat_ee = vm["fee"].as<string>();
        } 

        if (vm.count("fie")) {
			fwmat_ie = vm["fie"].as<string>();
        } 

        if (vm.count("fei")) {
			fwmat_ei = vm["fei"].as<string>();
        } 

        if (vm.count("fii")) {
			fwmat_ii = vm["fii"].as<string>();
        } 

    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }

	// BEGIN Global stuff
	mpi::environment env(ac, av);
	mpi::communicator world;
	communicator = &world;

	oss << dir  << "/coba." << world.rank() << ".";
	string outputfile = oss.str();

	char tmp [255];
	stringstream logfile;
	logfile << outputfile << "log";
	logger = new Logger(logfile.str(),world.rank(),PROGRESS,EVERYTHING);

	sys = new System(&world);
	// END Global stuff


	// double primetime = 10;
	logger->msg("Setting up neuron groups ...",PROGRESS,true);

	TIFGroup * neurons_e = new TIFGroup( ne);
	TIFGroup * neurons_i = new TIFGroup( ni);

	neurons_e->random_mem(-70e-3,10e-3);
	neurons_i->random_mem(-70e-3,10e-3);


	logger->msg("Setting up E connections ...",PROGRESS,true);

	SparseConnection * con_ee 
		= new SparseConnection( neurons_e,neurons_e, w, sparseness, GLUT);

	SparseConnection * con_ei 
		= new SparseConnection( neurons_e,neurons_i, w,sparseness,GLUT);



	logger->msg("Setting up I connections ...",PROGRESS,true);
	SparseConnection * con_ie 
		= new SparseConnection( neurons_i,neurons_e,wi,sparseness,GABA);

	SparseConnection * con_ii 
		= new SparseConnection( neurons_i,neurons_i,wi,sparseness,GABA);

	if ( prime ) { 
		msg = "Setting up external input ...";
		logger->msg(msg,PROGRESS,true);

		PoissonGroup * poisson= new PoissonGroup(200,10);
		poisson->seed(257); 
		// this will give the same seed on each rank,
		// but since the group should be locked to a single
		// rank we do not care.

		SparseConnection * con_ext_e = new SparseConnection(poisson,neurons_e,1,sparseness/2,GLUT);
		SparseConnection * con_ext_i = new SparseConnection(poisson,neurons_i,1,sparseness/2,GLUT);

	} else {
		oss.str("");
		oss << dir << "/save";
		sys->load_network_state(oss.str());
	}

	if ( !fwmat_ee.empty() ) con_ee->load_from_file(fwmat_ee);
	if ( !fwmat_ei.empty() ) con_ei->load_from_file(fwmat_ei);
	if ( !fwmat_ie.empty() ) con_ie->load_from_file(fwmat_ie);
	if ( !fwmat_ii.empty() ) con_ii->load_from_file(fwmat_ii);


	// pruning here impairs performance -- probably due to cache poisoning
	con_ee->prune();
	con_ei->prune();
	con_ie->prune();
	con_ii->prune();


	if ( !fast ) {
		msg = "Setting up monitors ...";
		logger->msg(msg,PROGRESS,true);

		stringstream filename;
		filename << outputfile << "e.ras";
		SpikeMonitor * smon_e = new SpikeMonitor( neurons_e, filename.str().c_str() );

		filename.str("");
		filename.clear();
		filename << outputfile << "i.ras";
		SpikeMonitor * smon_i = new SpikeMonitor( neurons_i, filename.str().c_str() );
		
		filename.str("");
		filename << outputfile << "e.mem";
		StateMonitor * smon_mem = new StateMonitor( neurons_e, 3, "mem", filename.str() ); 
		
		filename.str("");
		filename << outputfile << "e.ampa";
		StateMonitor * smon_ampa = new StateMonitor( neurons_e, 3, "g_ampa", filename.str() );

		filename.str("");
		filename << outputfile << "e.gaba";
		StateMonitor * smon_gaba = new StateMonitor( neurons_e, 3, "g_gaba", filename.str() );
	}


	RateChecker * chk = new RateChecker( neurons_e , -0.1 , 1000. , 100e-3);

	logger->msg("Running sanity check ...",PROGRESS,true);
	con_ee->sanity_check();
	con_ei->sanity_check();
	con_ie->sanity_check();
	con_ii->sanity_check();

	logger->msg("Simulating ..." ,PROGRESS,true);
	if (!sys->run(simtime,true)) 
			errcode = 1;

	if ( prime ) { 
		oss.str("");
		oss << dir << "/save";
		sys->save_network_state(oss.str());
	}

	logger->msg("Freeing ..." ,PROGRESS,true);
	delete sys;

	if (errcode)
		env.abort(errcode);

	return errcode;
}
