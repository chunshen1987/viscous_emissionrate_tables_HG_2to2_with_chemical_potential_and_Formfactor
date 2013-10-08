#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <gsl/gsl_integration.h>

#include "HG_2to2_Scattering.h"
#include "ParameterReader.h"
#include "Physicalconstants.h"
#include "gauss_quadrature.h"
#include "Arsenal.h"

using namespace std;

HG_2to2_Scattering::HG_2to2_Scattering(ParameterReader* paraRdr_in)
{
   paraRdr = paraRdr_in;

}

HG_2to2_Scattering::~HG_2to2_Scattering()
{

}

void HG_2to2_Scattering::output_emissionrateTable()
{

}
