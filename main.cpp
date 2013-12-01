#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Arsenal.h"
#include "Physicalconstants.h"
#include "Stopwatch.h"
#include "chemical_potential.h"
#include "ParameterReader.h"
#include "HG_2to2_Scattering.h"

using namespace std;


int main(int argc, char** argv)
{
   Stopwatch sw; 
   sw.tic();

   Chemical_potential* chempotential_ptr;
   Chemical_potential chemicalpotenital;
   chempotential_ptr = &chemicalpotenital;
   chempotential_ptr->readin_chempotential_table("chemical_potential_tb/s95p/s95p-PCE165-v0/s95p-v0-PCE165_chemvsT.dat");
   chempotential_ptr->Set_chemical_potential_s95pv0PCE();

   int channel = 0;
   string filename;
   
   ParameterReader* paraRdr = new ParameterReader();
   paraRdr->readFromFile("parameters.dat");
   paraRdr->readFromArguments(argc, argv);

   HG_2to2_Scattering HG2to2Rates(paraRdr);
   
/**********************************************************************/
// passed test
/**********************************************************************/
   
   int status;
   //C.1
   filename = "pion_rho_to_pion_gamma";
   channel = 1;
   status = HG2to2Rates.Calculate_emissionrates(chempotential_ptr, channel, filename);
   
   //C.1 omega
   filename = "pion_rho_to_omega_to_pion_gamma";
   channel = 2;
   status = HG2to2Rates.Calculate_emissionrates(chempotential_ptr, channel, filename);
  
   //C.2
   filename = "pion_pion_to_rho_gamma";
   channel = 3;
   status = HG2to2Rates.Calculate_emissionrates(chempotential_ptr, channel, filename);

   //C.4
   filename = "pion_Kstar_to_K_gamma";
   channel = 4;
   status = HG2to2Rates.Calculate_emissionrates(chempotential_ptr, channel, filename);

   //C.5
   filename = "pion_K_to_Kstar_gamma";
   channel = 5;
   status = HG2to2Rates.Calculate_emissionrates(chempotential_ptr, channel, filename);

   //C.6
   filename = "rho_K_to_K_gamma";
   channel = 6;
   status = HG2to2Rates.Calculate_emissionrates(chempotential_ptr, channel, filename);

   //C.7
   filename = "K_Kstar_to_pion_gamma";
   channel = 7;
   status = HG2to2Rates.Calculate_emissionrates(chempotential_ptr, channel, filename);

   sw.toc();
   cout << "totally takes : " << sw.takeTime() << "sec." << endl;
   return 0;
}
