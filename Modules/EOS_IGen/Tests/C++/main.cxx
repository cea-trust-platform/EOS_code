/****************************************************************************
* Copyright (c) 2023, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/



#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS_IGen/API/EOS_IGen.hxx"
#include <vector>

using namespace NEPTUNE;
using namespace NEPTUNE_EOS_IGEN;

int main()
{
	Language_init();

	{
		std::cout << "----------------------------------"<< std::endl;
		std::cout << "-------------- C++ ---------------"<< std::endl;
		std::cout << "--------- EOS_IGen      ----------"<< std::endl;
		std::cout << "----------------------------------"<< std::endl;

	}

	/**********************************
	 * EOS_IGen : Test 1er constructeur + Maillage avec raffinement (1)
	 **********************************/
	{
		cout << "#############################" << endl;
		cout << "EOS_IGen : 1er CONSTRUCTEUR"   << endl;
		cout << "#############################" << endl;

	}
	
	int nb_methodes=0;
 	Strings methodes(0);
	Strings references(0);
	EOS_Error Err;
	
#ifdef WITH_PLUGIN_THETIS
	nb_methodes=nb_methodes+1;
	methodes.resize(nb_methodes);
	references.resize(nb_methodes);
	methodes[nb_methodes-1]="EOS_Thetis";
	references[nb_methodes-1]="WaterLiquid";
#endif
#ifdef WITH_PLUGIN_REFPROP_9
	nb_methodes=nb_methodes+1;
	methodes.resize(nb_methodes);
	references.resize(nb_methodes);
	methodes[nb_methodes-1]="EOS_Refprop9";
	references[nb_methodes-1]="WaterLiquid";
#endif
#ifdef WITH_PLUGIN_REFPROP_10
	nb_methodes=nb_methodes+1;
	methodes.resize(nb_methodes);
	references.resize(nb_methodes);
	methodes[nb_methodes-1]="EOS_Refprop10";
	references[nb_methodes-1]="WaterLiquid";
#endif
#ifdef WITH_PLUGIN_CATHARE2
	nb_methodes=nb_methodes+1;
	methodes.resize(nb_methodes);
	references.resize(nb_methodes);
	methodes[nb_methodes-1]="EOS_Cathare2";
	references[nb_methodes-1]="WaterLiquid";
#endif
	for (int i=0; i<nb_methodes ; i++)
	{
// // 	  Sans raffinement
	  cout<<methodes[i]<<" : Sans raffinement"<<endl;
	  {
	    EOS_IGen obj_igen;
	    obj_igen.set_method(methodes[i].aschar());
	    obj_igen.set_reference(references[i].aschar());
	    
	    double pmin = 1.0e7;
	    double pmax = 2.0e7;
	    double Tmin = 300.e0;
	    double Tmax = 500.e0;
	    obj_igen.set_extremum(pmin,pmax,Tmin,Tmax);
	    
	    int nb_node_p=5;
	    int nb_node_h=5;

	    Err = obj_igen.make_mesh(nb_node_p,nb_node_h);
	    if (Err!=good)
	    {
	      cerr<<"Error to make mesh"<<endl;
	      exit(Err);
	    }
	    obj_igen.set_quality("rho","centre",1);
	    Err = obj_igen.compute_qualities();
	    if (Err!=good)
	    {
	      cerr<<"Error to compute qualities"<<endl;
	      exit(Err);
	    }
	    AString file_name="sans_raffinement_";
	    file_name+=methodes[i];
	    obj_igen.set_file_med_name(file_name);
	    Err = obj_igen.write_med();
	    if (Err!=good)
	    {
	      cerr<<"Error to write med file"<<endl;
	      exit(Err);
	    }
	    
	  }
// //  Avec raffinement global
	  cout<<methodes[i]<<" : Avec raffinement global"<<endl;
	  {
	    EOS_IGen obj_igen(methodes[i].aschar(),references[i].aschar());
  
	    double pmin = 1.0e6;
	    double pmax = 2.0e6;
	    double Tmin = 300.e0;
	    double Tmax = 400.e0;
	    obj_igen.set_extremum(pmin,pmax,Tmin,Tmax);
	    
	    int nb_node_p=5;
	    int nb_node_h=4;
	    
	    Err = obj_igen.make_mesh(nb_node_p,nb_node_h, 1);
	    if (Err!=good)
	    {
	      cerr<<"Error to make mesh"<<endl;
	      exit(Err);
	    }
	    
	    obj_igen.set_quality("mu","centre",0, 0.0002);

	    Err = obj_igen.make_global_refine();
	    if (Err!=good)
	    {
	      cerr<<"Error to make a global refine"<<endl;
	      exit(Err);
	    }
	    
	    AString file_name="raffinement_global_";
	    file_name+=methodes[i];
	    obj_igen.set_file_med_name(file_name);
	    
	    Err = obj_igen.write_med();
	    if (Err!=good)
	    {
	      cerr<<"Error to write med file"<<endl;
	      exit(Err);
	    }
	  }
	  
// 	  Avec raffinement local
	  cout<<methodes[i]<<" : Avec raffinement local"<<endl;
	  {
	    EOS_IGen obj_igen;
	    
	    obj_igen.set_method(methodes[i].aschar());
	    obj_igen.set_reference(references[i].aschar());
	    
	    
	    double pmin = 1.0e7;
	    double pmax = 2.0e7;
	    double Tmin = 300.e0;
	    double Tmax = 500.e0;
	    obj_igen.set_extremum(pmin,pmax,Tmin,Tmax);
	    
	    int nb_node_p=5;
	    int nb_node_h=5;
	    
	    Err = obj_igen.make_mesh(nb_node_p,nb_node_h, 3);
	    if (Err!=good)
	    {
	      cerr<<"Error to make mesh"<<endl;
	      exit(Err);
	    }
	    
	    obj_igen.set_quality("T","centre",0, 0.00012);

	    Err = obj_igen.make_local_refine();
	    if (Err!=good)
	    {
	      cerr<<"Error to make a local refine"<<endl;
	      exit(Err);
	    }
	    
	    AString file_name="raffinement_local_";
	    file_name+=methodes[i];
	    obj_igen.set_file_med_name(file_name);
	    
	    Err = obj_igen.write_med();
	    if (Err!=good)
	    {
	      cerr<<"Error to write med file"<<endl;
	      exit(Err);
	    }
	  }
// 	  Avec raffinement local non continu
// 	  utiliser pour le test de continuite de l'interpolateur
	  cout<<methodes[i]<<" : Avec raffinement local non continu"<<endl;
	  {
	    EOS_IGen obj_igen;
	    
	    obj_igen.set_method(methodes[i].aschar());
	    obj_igen.set_reference(references[i].aschar());
	    
	    
	    double pmin = 1.0e7;
	    double pmax = 2.0e7;
	    double Tmin = 300.e0;
	    double Tmax = 500.e0;
	    obj_igen.set_extremum(pmin,pmax,Tmin,Tmax);
	    
	    int nb_node_p=5;
	    int nb_node_h=5;
	    
	    Err = obj_igen.make_mesh(nb_node_p,nb_node_h, 3);
	    if (Err!=good)
	    {
	      cerr<<"Error to make mesh"<<endl;
	      exit(Err);
	    }
	    
	    obj_igen.set_quality("T","centre",0, 0.00012);

	    Err = obj_igen.make_local_refine(false);
	    if (Err!=good)
	    {
	      cerr<<"Error to make a local refine"<<endl;
	      exit(Err);
	    }
	    
	    AString file_name="raffinement_local_non_continu_";
	    file_name+=methodes[i];
	    obj_igen.set_file_med_name(file_name);
	    
	    Err = obj_igen.write_med();
	    if (Err!=good)
	    {
	      cerr<<"Error to write med file"<<endl;
	      exit(Err);
	    }
	  }
	}
	  
	
Language_finalize();
return 0;
}
