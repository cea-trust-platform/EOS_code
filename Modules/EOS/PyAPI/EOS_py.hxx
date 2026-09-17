#ifndef EOS_PY_HXX
#define EOS_PY_HXX

#include <vector>
#include <string>
#include "EOS/API/EOS.hxx"
#include <sstream>

class EOS_py {
public:
    EOS_py(const std::string& meth);
    EOS_py(const std::string& meth, const std::string& ref);
    ~EOS_py();

    std::vector<std::vector<double>> compute(
        std::string str_P,
        std::string str_H,
        std::vector<double> tab_P,
        std::vector<double> tab_H,
        std::vector<std::string> str_out
    );

    std::vector<std::vector<double>> compute_sat(
        std::string str_P,
        std::vector<double> tab_P,
        std::vector<std::string> str_out
    );
    std::string describe() const;
    static void fluids_available(); 

    double get_p_crit() const;
    double get_h_crit() const;
    double get_T_crit() const;
    double get_rho_crit() const;

    double get_p_min() const;
    double get_p_max() const;

    double get_h_max() const;
    double get_h_min() const;

    double get_T_max() const;
    double get_T_min() const;

    double get_rho_max() const;
    double get_rho_min() const;

    double get_p() const;
    double get_mm() const;

private:
    NEPTUNE::EOS* eos_;

    // Helper générique pour éviter duplication
    double call_getter(
        NEPTUNE::EOS_Error (NEPTUNE::EOS::*func)(double&) const,
        const std::string& name
    ) const;
};


class EOS_Mixing_py {
public:
    EOS_Mixing_py(
        std::vector<std::string> methods,
        std::vector<std::string> refs
    );

    ~EOS_Mixing_py();

    std::vector<std::vector<double>> compute(
        std::vector<std::string> input_names,
        std::vector<std::vector<double>> input_values,
        std::vector<std::string> output_names
    );
    std::string describe() const;

private:
    NEPTUNE::EOS* mixing_;
    std::vector<NEPTUNE::EOS*> components_;
};



#endif
