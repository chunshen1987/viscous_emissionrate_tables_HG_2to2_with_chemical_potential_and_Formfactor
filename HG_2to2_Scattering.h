#ifndef HG_2to2_Scattering_H
#define HG_2to2_Scattering_H

#include <string>
#include "ParameterReader.h"
#include "Physicalconstants.h"

class HG_2to2_Scattering;

struct CCallbackHolder
{
   HG_2to2_Scattering* clsPtr;
   void *params;
};

class HG_2to2_Scattering
{
   private:
      ParameterReader *paraRdr;

      Physicalconstants Phycons;

      int n_Eq, n_Temp;
      double *Eq_tb, *T_tb;
      double **equilibrium_results, **viscous_results;

      int channel;
      string filename;

   public:
      HG_2to2_Scattering(ParameterReader* paraRdr_in);
      ~HG_2to2_Scattering();
      
      void output_emissionrateTable();

};

#endif
