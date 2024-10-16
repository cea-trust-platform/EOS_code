
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Field.hxx"

using namespace NEPTUNE;
#include <iostream>
#include <iomanip>
#include <fstream>

#include <string>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif
#include "dump.hxx"
#include "arguments.hxx"
#include "timer.hxx"

const char *list_prop1[] = {
    "p"};
const char *list_prop2[] = {
    "h",
    "T"};

static vector<string> thermprop_r =
    {"T",
     "rho",
     "u",
     "s",
     "mu",
     "lambda",
     "cp",
     "sigma",
     "w",
     "g",
     "f",
     "pr",
     "beta",
     "gamma",
     "d_T_d_p_h",
     "d_T_d_h_p",
     "d_rho_d_p_h",
     "d_rho_d_h_p",
     "d_u_d_p_h",
     "d_u_d_h_p",
     "d_s_d_p_h",
     "d_s_d_h_p",
     "d_mu_d_p_h",
     "d_mu_d_h_p",
     "d_lambda_d_p_h",
     "d_lambda_d_h_p",
     "d_cp_d_p_h",
     "d_cp_d_h_p",
     "d_sigma_d_p_h",
     "d_sigma_d_h_p",
     "d_w_d_p_h",
     "d_w_d_h_p",
     "d_g_d_p_h",
     "d_g_d_h_p",
     "d_f_d_p_h",
     "d_f_d_h_p",
     "d_pr_d_p_h",
     "d_pr_d_h_p",
     "d_beta_d_p_h",
     "d_beta_d_h_p",
     "d_gamma_d_p_h",
     "d_gamma_d_h_p",
     "d_rho_d_T_p",
     "d_u_d_T_p",
     "d_s_d_T_p",
     "d_mu_d_T_p",
     "d_lambda_d_T_p",
     "d_cp_d_T_p",
     "d_sigma_d_T_p",
     "d_w_d_T_p",
     "d_g_d_T_p",
     "d_f_d_T_p",
     "d_pr_d_T_p",
     "d_beta_d_T_p",
     "d_gamma_d_T_p",
     "d_h_d_T_p",
     "d_h_d_p_T"};

static vector<string> saturprop_r =
    {"rho_l_sat",
     "rho_v_sat",
     "h_l_sat",
     "h_v_sat",
     "cp_l_sat",
     "cp_v_sat",
     "T_sat",
     "d_rho_l_sat_d_p",
     "d_rho_v_sat_d_p",
     "d_h_l_sat_d_p",
     "d_h_v_sat_d_p",
     "d_cp_l_sat_d_p",
     "d_cp_v_sat_d_p",
     "d_T_sat_d_p",
     "d2_T_sat_d_p_d_p"};

void printError(const EOS &eos,
                const EOS_Internal_Error err)
{
  AString s;
  eos.fluid().describe_error(err, s);
  std::cerr << "error " << err.get_partial_code() << " " << s << std::endl;
}

void test_field_1parameter(EOS &eos, const char *valIn, int iSample, bool dump, int nThreads)
{
  std::ofstream fOut;
  if (dump)
  {
    std::string s = std::to_string(nThreads) + "_threads_" 
                  + valIn + std::to_string(iSample) + ".data";
    fOut.open(s.c_str());
  }

  ArrOfDouble array(1);
  array[0] = 1e5 + iSample * 3e1;
  EOS_Field fieldIn(valIn, valIn, array);

  ArrOfDouble result(1);
  ArrOfInt errors(1);
  EOS_Error_Field err_field(errors);

  if (dump)
  {
    fOut << std::endl
         << valIn << " " << fieldIn[0] << std::endl;
  }

  for (auto p : saturprop_r)
  {
    EOS_Field fieldOut(p.c_str(), p.c_str(), result);

    // Test field function
    eos.compute(fieldIn, fieldOut, err_field);
    NEPTUNE::EOS_Internal_Error err = err_field[0];

    if (err.generic_error() != EOS_Error::good)
    {
#ifdef _OPENMP
#pragma omp critical
#endif
      {
        printError(eos, err);
      }
    }

    if (dump)
      fOut << "\t" << std::setfill(' ') << std::setw(25) << std::right << p
           << " "  << std::setfill('_') << std::setw(25) << std::right << std::setprecision(15) << fieldOut[0] << std::endl;
  }
}

void test_field_2parameters(EOS &eos, const char *valIn1, const char *valIn2, int iSample, bool dump, int nThreads)
{
  std::ofstream fOut;
  if (dump)
  {
    std::string s = std::to_string(nThreads) + "_threads_" 
                  + valIn1 + valIn2 + std::to_string(iSample) + ".data";
    fOut.open(s.c_str());
  }

  ArrOfDouble array1(1);
  ArrOfDouble array2(1);
  ArrOfDouble result(1);
  ArrOfInt errors(1);

  array1[0] = 1e5 + iSample * 3e1;

  if (strcmp(valIn2, "T") == 0)
    array2[0] = 350. + iSample * 0.1;
  if (strcmp(valIn2, "h") == 0)
    array2[0] = 210000. + iSample * 100;

  EOS_Field fieldIn1(valIn1, valIn1, array1);
  EOS_Field fieldIn2(valIn2, valIn2, array2);

  EOS_Error_Field err_field(errors);

  if (dump)
  {
    fOut << std::endl
         << valIn1 << " " << std::setprecision(15) << fieldIn1[0] << " " 
         << valIn2 << " " << std::setprecision(15) << fieldIn2[0] << std::endl;
  }

  for (auto p : thermprop_r)
  {
    EOS_Field fieldOut(p.c_str(), p.c_str(), result);
    eos.compute(fieldIn1, fieldIn2, fieldOut, err_field);
    NEPTUNE::EOS_Internal_Error err = err_field[0];

    if (err.generic_error() != EOS_Error::good)
    {
#ifdef _OPENMP
#pragma omp critical
#endif
      {
        printError(eos, err);
      }
    }

    if (dump)
      fOut << "\t" << std::setfill(' ') << std::setw(25) << std::right << p
           << " "  << std::setfill('_') << std::setw(25) << std::setprecision(15) << std::right << fieldOut[0] << std::endl;
  }
}

void test_point_1parameter(EOS &eos, int iSample, bool dump, int nThreads)
{
  std::ofstream fOut;
  if (dump)
  {
    std::string s = std::to_string(nThreads) + "_threads_point_p_";
    s += std::to_string(iSample) + ".data";
    fOut.open(s.c_str());
  }

  for (auto q : saturprop_r)
  {
    double p = 1.5e5 + iSample * 3e1;
    double r = 0.e0;
    EOS_Internal_Error ierr = EOS_Internal_Error::OK;

    try
    { // EOS will throw an EOS_Std_Exception if an error occurs:
      eos.compute(q.c_str(), p, r);
      if (dump)
        fOut << "\t" << std::setfill(' ') << std::setw(25) << std::right << q
             << " "  << std::setfill('_') << std::setw(25) << std::setprecision(15) << std::right << r << std::endl;
    }
    catch (EOS_Std_Exception ex)
    { // Get the internal error code:
      ierr = ex.err_code;
    }
  }
}

void test_point_2parameters(EOS &eos, int iSample, bool dump, int nThreads)
{
  std::ofstream fOut;
  if (dump)
  {
    std::string s = std::to_string(nThreads) + "_threads_point_p_h_";
    s += std::to_string(iSample) + ".data";
    fOut.open(s.c_str());
  }

  for (auto q : thermprop_r)
  {
    double p = 1.5e5 + iSample * 3e1;
    double h = 300.0 + iSample * 0.05;
    double r = 0.e0;
    EOS_Internal_Error ierr = EOS_Internal_Error::OK;

    try
    { // EOS will throw an EOS_Std_Exception if an error occurs:
      eos.compute(q.c_str(), p, h, r);
      if (dump)
        fOut << "\t" << std::setfill(' ') << std::setw(25) << std::right << q
             << " "  << std::setfill('_') << std::setw(25) << std::setprecision(15) << std::right << r << std::endl;
    }
    catch (EOS_Std_Exception ex)
    { // Get the internal error code:
      ierr = ex.err_code;
    }
  }
}

void test_features(EOS &eos, int test, int nSamples, bool dump, int nThreads)
{
  // Configure eos to return to client on any error.
  std::cout << eos << std::endl;

  EOS_Std_Error_Handler handler;
  // EOS should never exit()
  handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature);
  // EOS should never throw() an exception
  handler.set_throw_on_error(EOS_Std_Error_Handler::disable_feature);
  // EOS should dump informations to std::cerr on error,bad and ok
  handler.set_dump_on_error(ok);
  handler.set_dump_stream(std::cerr);

  // eos provides a stack of saved error handlers which
  // is used if we want to locally change the error handler.
  // save_error_handler() must match a restore_error_handler() somewhere...
  eos.save_error_handler();
  eos.set_error_handler(handler);

  if (test == 1)
  {
    Timer T("Testing field methods with 1 parameter  :");
    int n1 = sizeof(list_prop1) / sizeof(char *);

    T.start();

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int iSample = 0; iSample < nSamples; iSample++)
      for (int i = 0; i < n1; i++)
        test_field_1parameter(eos, list_prop1[i], iSample, dump, nThreads);

    T.stop();
    std::cout << T << std::endl;
  }

  if (test == 2)
  {
    Timer T("Testing field methods with 2 parameters :");
    int n1 = sizeof(list_prop1) / sizeof(char *);
    int n2 = sizeof(list_prop2) / sizeof(char *);

    T.start();

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int iSample = 0; iSample < nSamples; iSample++)
      for (int i = 0; i < n1; i++)
        for (int j = 0; j < n2; j++)
          test_field_2parameters(eos, list_prop1[i], list_prop2[j], iSample, dump, nThreads);

    T.stop();
    std::cout << T << std::endl;
  }

  if (test == 3)
  {
    handler.set_throw_on_error(ok);
    eos.set_error_handler(handler);

    Timer T("Testing point methods with 1 parameter  :");

    T.start();

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int iSample = 0; iSample < nSamples; iSample++)
    {
      test_point_1parameter(eos, iSample, dump, nThreads);
    }

    T.stop();
    std::cout << T << std::endl;
  }

  if (test == 4)
  {
    handler.set_throw_on_error(ok);
    eos.set_error_handler(handler);

    Timer T("Testing point methods with 2 parameters :");

    T.start();

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int iSample = 0; iSample < nSamples; iSample++)
    {
      test_point_2parameters(eos, iSample, dump, nThreads);
    }

    T.stop();
    std::cout << T << std::endl;
  }

  // Reset to previous handler
  eos.restore_error_handler();
}

int main(int argc, char **argv)
{
  int nTh;

#ifdef _OPENMP
#pragma omp parallel
#pragma omp single
  {
    nTh = omp_get_num_threads();
  }
  std::cout << nTh << " thread(s)" << std::endl;
#else
  nTh = 1;
  std::cout << "sequential" << std::endl;
#endif

  Arguments A(argc, argv);

  int nSamples = A.Get("samples", nTh*3);
  bool dump = A.Get("dump", false);
  std::string plugin = A.Get("plugin", "Refprop10");
  std::string fluid = A.Get("fluid", "Water");
  bool h = A.Get("help", false);
  if (h) {
    std::cout << A << std::endl;
    return -1;
  }

  std::cout << nSamples << " sample(s)" << std::endl;

  std::cout << "dump : " << dump << std::endl
            << std::endl;

  cDump D;
  if (dump)
    D.activate();

  std::cout<<endl<<endl;
  std::cout<<"--------------------------------------- "<<std::endl;
  std::cout<<"---------------- Begin ---------------- "<<std::endl;
  std::cout<<"--------------------------------------- "<<std::endl;

  Language_init();


  std::cout<<endl<<"--------------------------------------- "<<std::endl;
  std::cout<<      "------ Class available ...  ----------- "<<std::endl<<std::endl;

  hierrarchy();

  EOS eos(plugin.c_str(), fluid.c_str());

  for (int test=1; test <= 4; test++)
    test_features(eos, test, nSamples, dump, nTh);
  
  return 0;
}
