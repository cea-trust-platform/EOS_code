#include "dump.hxx"
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

void test_1parameter(EOS &eos, const char *valIn, int iSample, bool dump)
{
  std::ofstream fOut;
  if (dump)
  {
    std::string s = valIn + std::to_string(iSample) + ".data";
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
      fOut << "\t" << std::setfill(' ') << std::setw(17) << std::right << p
           << std::setfill('_') << std::setw(22) << std::right << fieldOut[0] << std::endl;
  }
}

void test_2parameters(EOS &/*eos*/, const char */*valIn1*/, const char */*valIn2*/, int /*iSample*/, bool /*dump*/)
{
}

void test_features(EOS &eos, int nSamples, bool dump)
{
  // Configure eos to return to client on any error.
  std::cout << eos << std::endl;
  std::cout << "Testing field methods with 1 parameter" << endl;

  EOS_Std_Error_Handler handler;
  // EOS should never exit()
  handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature);
  // EOS should never throw() an exception
  handler.set_throw_on_error(EOS_Std_Error_Handler::disable_feature);
  // EOS should dump informations to std::cerr on error,bad and ok
  handler.set_dump_on_error(ok);
  handler.set_dump_stream(std::cout);

  // eos provides a stack of saved error handlers which
  // is used if we want to locally change the error handler.
  // save_error_handler() must match a restore_error_handler() somewhere...
  eos.save_error_handler();
  eos.set_error_handler(handler);

  int iSample;
  int n1 = sizeof(list_prop1) / sizeof(char *);
  int n2 = sizeof(list_prop2) / sizeof(char *);

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (iSample = 0; iSample < nSamples; iSample++)
  {
    for (int i = 0; i < n1; i++)
    {
      test_1parameter(eos, list_prop1[i], iSample, dump);
    }
  }

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (iSample = 0; iSample < nSamples; iSample++)
  {
    for (int i = 0; i < n1; i++)
    {
      for (int j = 0; j < n2; j++)
      {
        test_2parameters(eos, list_prop1[i], list_prop2[j], iSample, dump);
      }
    }
    // Reset to previous handler
    eos.restore_error_handler();
  }
}

int main(int argc, char **argv)
{
  int nSamples = argc > 1 ? strtol(argv[1], nullptr, 10) : 10;
  bool dump = argc > 2 ? (strcmp(argv[2], "dump") == 0) : false;

  cDump D;
  if (dump)
    D.activate();

  EOS fluid("EOS_Refprop10", "Water");

  test_features(fluid, nSamples, dump);

  return 0;
}
