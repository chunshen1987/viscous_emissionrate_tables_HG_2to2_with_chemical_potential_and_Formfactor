#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Phasespace_integrals.h"
using namespace std;

int Calculate_emissionrates(double* m, Chemical_potential* chempotential_ptr, int channel, string filename)
{
   double* results = new double [2];
   double* s_pt = new double [n_s];
   double* s_weight = new double [n_s];

   double** t_pt = new double* [n_s];
   double** t_weight = new double* [n_s];
   double** Matrix_elements_sq_ptr = new double* [n_s];
   for(int i=0; i<n_s; i++)
   {
       t_pt[i] = new double [n_t];
       t_weight[i] = new double [n_t];
       Matrix_elements_sq_ptr[i] = new double [n_t];
   }
   
   double* E1_pt_standard = new double [n_E1];
   double* E1_weight_standard = new double [n_E1];
   double** E2_pt_standard = new double* [3];
   double** E2_weight_standard = new double* [3];
   for(int i=0; i<3; i++)
   {
      E2_pt_standard[i] = new double [n_E2];
      E2_weight_standard[i] = new double [n_E2];
   }

   set_gausspoints(m, s_pt, s_weight, t_pt, t_weight, E1_pt_standard, E1_weight_standard, E2_pt_standard, E2_weight_standard);
   
   // calculate matrix elements squared
   for(int i=0; i<n_s; i++)
      for(int j=0; j<n_t; j++)
           Matrix_elements_sq_ptr[i][j] = Matrix_elements_sq(s_pt[i], t_pt[i][j], channel);

   double* Eq_tb = new double [n_Eq];
   double* Formfactor_tb = new double [n_Eq];
   double* T_tb = new double [n_Temp];
   double* mu1_tb = new double [n_Temp];
   double* mu2_tb = new double [n_Temp];
   double* mu3_tb = new double [n_Temp];
   for(int i=0; i<n_Eq; i++)
      Eq_tb[i] = Eq_i + i*dEq;
   for(int i=0; i<n_Temp; i++)
      T_tb[i] = T_i + i*dT;

   // calculate form factor
   Calculate_Formfactor(Eq_tb, Formfactor_tb, n_Eq, channel);
   // calculate chemical potenitals
   chempotential_ptr->Calculate_mu(T_tb, mu1_tb, mu2_tb, mu3_tb, n_Temp, channel);

   double** equilibrium_results = new double* [n_Eq];
   double** viscous_results = new double*[n_Eq];
   for(int i=0; i<n_Eq; i++)
   {
      equilibrium_results[i] = new double [n_Temp];
      viscous_results[i] = new double [n_Temp];
   }

   double Eq;
   double formfactor;
   double T;
   double* mu = new double [3];
   for(int j=0; j<n_Temp; j++)
   {
      T = T_tb[j];
      mu[0] = mu1_tb[j];
      mu[1] = mu2_tb[j];
      mu[2] = mu3_tb[j];
      for(int i=0; i<n_Eq; i++)
      {
          Eq = Eq_tb[i];
          formfactor = Formfactor_tb[i];
          double prefactor = 1./16./pow(2.0*M_PI, 7)/Eq*formfactor;

          double equilibrium_result_s = 0.0;
          double viscous_result_s = 0.0;
          for(int k=0; k<n_s; k++)
          {
             double equilibrium_result_t = 0.0;
             double viscous_result_t = 0.0;
             for(int l=0; l<n_t; l++)
             {
                Integrate_E1(m, mu, Eq, T, channel, s_pt[k], t_pt[k][l], E1_pt_standard, E1_weight_standard, E2_pt_standard, E2_weight_standard, results);
                equilibrium_result_t += Matrix_elements_sq_ptr[k][l]*results[0]*t_weight[k][l];
                viscous_result_t += Matrix_elements_sq_ptr[k][l]*results[1]*t_weight[k][l];
             }
             equilibrium_result_s += equilibrium_result_t*s_weight[k];
             viscous_result_s += viscous_result_t*s_weight[k];
          }
          equilibrium_results[i][j] = equilibrium_result_s*prefactor/pow(hbarC, 4); // convert units to 1/(GeV^2 fm^4) for the emission rates
          viscous_results[i][j] = viscous_result_s*prefactor/pow(hbarC, 4); // convert units to 1/(GeV^2 fm^4) for the emission rates
      }
   }

   // output emission rate tables
   ostringstream output_file_eqrate;
   ostringstream output_file_viscous;
   output_file_eqrate << "rate_" << filename << "_eqrate.dat";
   output_file_viscous << "rate_" << filename << "_viscous.dat";
   ofstream of_eqrate(output_file_eqrate.str().c_str());
   ofstream of_viscous(output_file_viscous.str().c_str());
   for(int j=0; j<n_Temp; j++)
   {
      for(int i=0; i<n_Eq; i++)
      {
         of_eqrate << scientific << setw(20) << setprecision(8)
                   << equilibrium_results[i][j] << "   ";
         of_viscous << scientific << setw(20) << setprecision(8)
                   << viscous_results[i][j] << "   ";
      }
      of_eqrate << endl;
      of_viscous << endl;
   }

   of_eqrate.close();
   of_viscous.close();

   delete[] results;
   delete[] s_pt;
   delete[] s_weight;
   for(int i=0; i<n_s; i++)
   {
      delete[] t_pt[i];
      delete[] t_weight[i];
      delete[] Matrix_elements_sq_ptr[i];
   }
   delete[] t_pt;
   delete[] t_weight;
   delete[] Matrix_elements_sq_ptr;
   
   delete[] E1_pt_standard;
   delete[] E1_weight_standard;
   for(int i=0; i<3; i++)
   {
      delete[] E2_pt_standard[i];
      delete[] E2_weight_standard[i];
   }
   delete[] E2_pt_standard;
   delete[] E2_weight_standard;

   delete[] Eq_tb;
   delete[] T_tb;
   delete[] mu1_tb;
   delete[] mu2_tb;
   delete[] mu3_tb;
   delete[] Formfactor_tb;
   delete[] mu;
   for(int i=0; i<n_Eq; i++)
   {
      delete[] equilibrium_results[i];
      delete[] viscous_results[i];
   }
   delete[] equilibrium_results;
   delete[] viscous_results;

   return(0);
}

double set_gausspoints(double* m, double* s_pt, double* s_weight, double** t_pt, double** t_weight, double* E1_pt_standard, double* E1_weight_standard, double** E2_pt_standard, double** E2_weight_standard)
{
   double s_min;
   double m_init_sq = (m[0] + m[1])*(m[0] + m[1]);
   double m_final_sq = m[2]*m[2];
   if(m_init_sq > m_final_sq)
      s_min = m_init_sq;
   else
      s_min = m_final_sq;
  
   gauss_quadrature(n_s, 1, 0.0, 0.0, s_min, s_max, s_pt, s_weight);
  
   for(int i=0; i<n_s; i++)
   {
      double s = s_pt[i];
      double t_min;
      double t_max;
      t_min = m[0]*m[0] + m[2]*m[2] - 2*(s + m[0]*m[0] - m[1]*m[1])*(s+m[2]*m[2])
              /4/s - 2*sqrt((s+m[0]*m[0]-m[1]*m[1])*(s+m[0]*m[0]-m[1]*m[1]) 
              - 4*s*m[0]*m[0])*(s - m[2]*m[2])/4/s;
      t_max = m[0]*m[0] + m[2]*m[2] - 2*(s + m[0]*m[0] - m[1]*m[1])*(s+m[2]*m[2])
              /4/s + 2*sqrt((s+m[0]*m[0]-m[1]*m[1])*(s+m[0]*m[0]-m[1]*m[1])
              - 4*s*m[0]*m[0])*(s - m[2]*m[2])/4/s;

      gauss_quadrature(n_t, 1, 0.0, 0.0, t_min, t_max, t_pt[i], t_weight[i]);
    }
    
    gauss_quadrature_standard(n_E1, 5, 0.0, 0.0, 0.0, 1.0, E1_pt_standard, E1_weight_standard);

    // use Chebyshev–Gauss quadrature for channels: pi + rho, pi + Kstar, rho + K, and K + Kstar
    gauss_quadrature_standard(n_E2, 2, 0.0, 0.0, 0.0, 1.0, E2_pt_standard[0], E2_weight_standard[0]);

    // use Jacobi-Gauss quadrature for channels: pi + pi, pi + K
    gauss_quadrature_standard(n_E2, 4, 0.0, -0.5, 0.0, 1.0, E2_pt_standard[1], E2_weight_standard[1]);
    gauss_quadrature_standard(n_E2, 4, -0.5, 0.0, 0.0, 1.0, E2_pt_standard[2], E2_weight_standard[2]);

    return(0);

}

double Integrate_E1(double* m, double* mu, double Eq, double T, int channel, double s, double t, double* E1_pt_standard, double* E1_weight_standard, double** E2_pt_standard, double** E2_weight_standard, double* results)
{
   double equilibrium_result = 0.0e0;
   double viscous_result = 0.0e0;
   double E1_min;
   double u = - s - t + m[0]*m[0] + m[1]*m[1] + m[2]*m[2];
   E1_min = Eq*m[0]*m[0]/(m[0]*m[0] - u) + (m[0]*m[0] - u)/4/Eq;

   double* E1_pt = new double [n_E1];
   double* E1_weight = new double [n_E1];
   for(int i=0; i<n_E1; i++)
   {
      E1_pt[i] = E1_pt_standard[i];
      E1_weight[i] = E1_weight_standard[i];
   }
   
   double slope = 1./T;
   scale_gausspoints(n_E1, 5, 0.0, 0.0, E1_min, slope, E1_pt, E1_weight);

   for(int i=0; i<n_E1; i++)
   {
      Integrate_E2(m, mu, Eq, T, channel, s, t, E1_pt[i], E2_pt_standard, E2_weight_standard, results);
      equilibrium_result += results[0]*E1_weight[i];
      viscous_result += results[1]*E1_weight[i];
   }

   results[0] = equilibrium_result;
   results[1] = viscous_result;

   delete[] E1_pt;
   delete[] E1_weight;

   return(0);
}


double Integrate_E2(double* m, double* mu, double Eq, double T, int channel, double s, double t, double E1, double** E2_pt_standard, double** E2_weight_standard, double* results)
{
   double eps = 1e-100;
   double equilibrium_result = 0.0;
   double viscous_result = 0.0;
   double E2_min;
   double E2_max;
   double min_1 = Eq*m[1]*m[1]/(m[1]*m[1] - t) + (m[1]*m[1] - t)/4/Eq;

   double a = - (s + t - m[1]*m[1] - m[2]*m[2])*(s + t - m[1]*m[1] - m[2]*m[2]);
   double b = Eq*((s + t - m[1]*m[1] - m[2]*m[2])*(s - m[0]*m[0] - m[1]*m[1]) 
              - 2*m[0]*m[0]*(m[1]*m[1] - t)) + E1*(m[1]*m[1] - t)
              *(s + t - m[1]*m[1] - m[2]*m[2]);
   double c = - (t - m[1]*m[1])*(t - m[1]*m[1])*E1*E1
              - 2*Eq*(2*m[1]*m[1]*(s + t - m[1]*m[1] - m[2]*m[2]) 
              - (m[1]*m[1] - t)*(s - m[0]*m[0] - m[1]*m[1]))*E1
              + 4*Eq*Eq*m[0]*m[0]*m[1]*m[1] + m[1]*m[1]*(s + t - m[1]*m[1] 
              - m[2]*m[2])*(s + t - m[1]*m[1] - m[2]*m[2]) + m[0]*m[0]
              *(m[1]*m[1] -t)*(m[1]*m[1] -t)
              - Eq*Eq*(s - m[0]*m[0] - m[1]*m[1])*(s - m[0]*m[0] - m[1]*m[1])
              + (s - m[0]*m[0] - m[1]*m[1])*(t - m[1]*m[1])*(s + t - m[1]*m[1] 
              - m[2]*m[2]);

   if((b*b - a*c) >= 0) 
   {
      double min_2 = (-b + sqrt(b*b - a*c))/(a + eps);
      if(min_1 < min_2)
         E2_min = min_2;
      else
         E2_min = min_1;
      E2_max = (-b - sqrt(b*b - a*c))/(a + eps);

      if(E2_max < E2_min)
      {
         results[0] = 0.0e0;
         results[1] = 0.0e0;
         return (0.0);
      }
   
      double mu1 = mu[0];
      double mu2 = mu[1];
      double mu3 = mu[2];
      double common_factor;

      double* E2_pt = new double [n_E2];
      double* E2_weight = new double [n_E2];

      
      if(channel == 3 || channel == 5)
      {
         double E2_cut = E2_min + (E2_max - E2_min)/100.;
         for(int i=0; i<n_E2; i++)
         {
            E2_pt[i] = E2_pt_standard[1][i];
            E2_weight[i] = E2_weight_standard[1][i];
         }
         scale_gausspoints(n_E2, 4, 0.0, -0.5, E2_min, E2_cut, E2_pt, E2_weight);
         for(int i=0; i<n_E2; i++)
         {
            double f0_E1 = Bose_distribution(E1, T, mu1);
            double f0_E2 = Bose_distribution(E2_pt[i], T, mu2);
            double f0_E3 = Bose_distribution(E1 + E2_pt[i] - Eq, T, mu3);
            common_factor = f0_E1*f0_E2*(1 + f0_E3)/(sqrt(a*E2_pt[i]*E2_pt[i] + 2*b*E2_pt[i] + c) + eps);
            equilibrium_result += common_factor*1.*E2_weight[i];
            viscous_result += common_factor*viscous_integrand(s, t, E1, E2_pt[i], Eq, T, m, f0_E1, f0_E2, f0_E3)*E2_weight[i];
         }

         for(int i=0; i<n_E2; i++)
         {
            E2_pt[i] = E2_pt_standard[2][i];
            E2_weight[i] = E2_weight_standard[2][i];
         }
         scale_gausspoints(n_E2, 4, -0.5, 0.0, E2_cut, E2_max, E2_pt, E2_weight);
         for(int i=0; i<n_E2; i++)
         {
            double f0_E1 = Bose_distribution(E1, T, mu1);
            double f0_E2 = Bose_distribution(E2_pt[i], T, mu2);
            double f0_E3 = Bose_distribution(E1 + E2_pt[i] - Eq, T, mu3);
            common_factor = f0_E1*f0_E2*(1 + f0_E3)/(sqrt(a*E2_pt[i]*E2_pt[i] + 2*b*E2_pt[i] + c) + eps);
            equilibrium_result += common_factor*1.*E2_weight[i];
            viscous_result += common_factor*viscous_integrand(s, t, E1, E2_pt[i], Eq, T, m, f0_E1, f0_E2, f0_E3)*E2_weight[i];
         }
      }
      else
      {
         for(int i=0; i<n_E2; i++)
         {
            E2_pt[i] = E2_pt_standard[0][i];
            E2_weight[i] = E2_weight_standard[0][i];
         }
         scale_gausspoints(n_E2, 2, 0.0, 0.0, E2_min, E2_max, E2_pt, E2_weight);
         for(int i=0; i<n_E2; i++)
         {
            double f0_E1 = Bose_distribution(E1, T, mu1);
            double f0_E2 = Bose_distribution(E2_pt[i], T, mu2);
            double f0_E3 = Bose_distribution(E1 + E2_pt[i] - Eq, T, mu3);
            common_factor = f0_E1*f0_E2*(1 + f0_E3)/(sqrt(a*E2_pt[i]*E2_pt[i] + 2*b*E2_pt[i] + c) + eps);
            equilibrium_result += common_factor*1.*E2_weight[i];
            viscous_result += common_factor*viscous_integrand(s, t, E1, E2_pt[i], Eq, T, m, f0_E1, f0_E2, f0_E3)*E2_weight[i];
         }
      }

      delete[] E2_pt;
      delete[] E2_weight;
   }
   else  // no kinematic phase space
   {
      equilibrium_result = 0.0e0;
      viscous_result = 0.0e0;
   }
   results[0] = equilibrium_result;
   results[1] = viscous_result;

   return(0);
}

inline double viscous_integrand(double s, double t, double E1, double E2, double Eq, double T, double* m, double f0_E1, double f0_E2, double f0_E3)
{
   double eps = 1e-100;
   double m1 = m[0];
   double m2 = m[1];
   double m3 = m[2];
   double E3 = E1 + E2 - Eq;
   double p1 = sqrt(E1*E1 - m1*m1);
   double p2 = sqrt(E2*E2 - m2*m2);
   double p3 = sqrt(E3*E3 - m3*m3);
   double costheta1 = (- s - t + m2*m2 + m3*m3 + 2*E1*Eq)/(2*p1*Eq + eps);
   double costheta2 = (t - m2*m2 + 2*E2*Eq)/(2*p2*Eq + eps);
   double p3_z = p1*costheta1 + p2*costheta2 - Eq; 
   
   double integrand = (1. + f0_E1)*deltaf_chi(p1/T)*0.5*(-1. + 3.*costheta1*costheta1) + (1. + f0_E2)*deltaf_chi(p2/T)*0.5*(-1. + 3.*costheta2*costheta2) + f0_E3*deltaf_chi(p3/T)/p3/p3*(-0.5*p3*p3 + 1.5*p3_z*p3_z);

   return(integrand);
}

inline double Bose_distribution(double E, double T, double mu)
{
   return(1.0/(exp((E-mu)/T)-1.0));
}

inline double deltaf_chi(double p)
{ 
    return(pow(p, deltaf_alpha));
}
