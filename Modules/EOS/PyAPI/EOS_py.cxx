#include "EOS_py.hxx"
#include "Language/API/UObject.hxx"

#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>


EOS_py::EOS_py(const std::string& meth) {
    eos_ = new NEPTUNE::EOS(meth.c_str());
}

EOS_py::EOS_py(const std::string& meth, const std::string& ref) {
    eos_ = new NEPTUNE::EOS(meth.c_str(), ref.c_str());
}

EOS_py::~EOS_py() {
    delete eos_;
}

std::vector<std::vector<double>> EOS_py::compute(
    std::string str_P,
    std::string str_H,
    std::vector<double> tab_P,
    std::vector<double> tab_H,
    std::vector<std::string> str_out
) {
    const unsigned int nb_calc = static_cast<unsigned int>(tab_P.size());
    const unsigned int nb_prop = static_cast<unsigned int>(str_out.size());

    if (tab_H.size() != nb_calc) {
        throw std::runtime_error("compute: tab_P and tab_H must have same size");
    }
    if (nb_calc == 0) {
        throw std::runtime_error("compute: empty input arrays");
    }
    if (nb_prop == 0) {
        throw std::runtime_error("compute: no output requested");
    }

    NEPTUNE::EOS_Field inputP("Input 1", str_P.c_str(), nb_calc, &tab_P[0]);
    NEPTUNE::EOS_Field inputH("Input 2", str_H.c_str(), nb_calc, &tab_H[0]);

    std::vector<double> outputs_vect(nb_prop * nb_calc);
    std::vector<NEPTUNE::ArrOfDouble> outputs_array(nb_prop);
    NEPTUNE::EOS_Fields outputs(nb_prop);

    for (unsigned int i = 0; i < nb_prop; i++) {
        outputs_array[i].set_ptr(nb_calc, &outputs_vect[i * nb_calc]);
        outputs[i].init(str_out[i].c_str(), str_out[i].c_str(), outputs_array[i]);
    }

    std::vector<int> error(nb_calc, 0);
    NEPTUNE::EOS_Error_Field eos_error_field(nb_calc, &error[0]);

    NEPTUNE::EOS_Error worst = eos_->compute(inputP, inputH, outputs, eos_error_field);
    if (worst != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error(
            "compute: bad computation, worst error = " + std::to_string(static_cast<int>(worst))
        );
    }

    std::vector<std::vector<double>> result(nb_calc, std::vector<double>(nb_prop));
    for (size_t j = 0; j < nb_calc; ++j) {
        for (size_t i = 0; i < nb_prop; ++i) {
            result[j][i] = outputs[i][j];
        }
    }

    return result;
}

std::vector<std::vector<double>> EOS_py::compute_sat(
    std::string str_P,
    std::vector<double> tab_P,
    std::vector<std::string> str_out
) {
    const unsigned int nb_calc = static_cast<unsigned int>(tab_P.size());
    const unsigned int nb_prop = static_cast<unsigned int>(str_out.size());

    if (nb_calc == 0) {
        throw std::runtime_error("compute_sat: empty input arrays");
    }
    if (nb_prop == 0) {
        throw std::runtime_error("compute_sat: no output requested");
    }

    NEPTUNE::EOS_Field inputP("Input 1", str_P.c_str(), nb_calc, &tab_P[0]);

    std::vector<double> outputs_vect(nb_prop * nb_calc);
    std::vector<NEPTUNE::ArrOfDouble> outputs_array(nb_prop);
    NEPTUNE::EOS_Fields outputs(nb_prop);

    for (unsigned int i = 0; i < nb_prop; i++) {
        outputs_array[i].set_ptr(nb_calc, &outputs_vect[i * nb_calc]);
        outputs[i].init(str_out[i].c_str(), str_out[i].c_str(), outputs_array[i]);
    }

    std::vector<int> error(nb_calc, 0);
    NEPTUNE::EOS_Error_Field eos_error_field(nb_calc, &error[0]);

    NEPTUNE::EOS_Error worst = eos_->compute(inputP, outputs, eos_error_field);
    if (worst != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error(
            "compute_sat: bad computation, worst error = " + std::to_string(static_cast<int>(worst))
        );
    }

    std::vector<std::vector<double>> result(nb_calc, std::vector<double>(nb_prop));
    for (size_t j = 0; j < nb_calc; ++j) {
        for (size_t i = 0; i < nb_prop; ++i) {
            result[j][i] = outputs[i][j];
        }
    }
        return result;
}

std::string EOS_py::describe() const {
    if (!eos_) {
        throw std::runtime_error("EOS_py::eos_ not initialized");
    }

    std::ostringstream oss;
    oss << (*eos_);  // appelle operator<< qui est surchargé par fluid_description

    return oss.str();
}

void EOS_py::fluids_available() {
    std::cout<<Types_Info::instance()<<std::endl;
}

double EOS_py::call_getter(
    NEPTUNE::EOS_Error (NEPTUNE::EOS::*func)(double&) const,
    const std::string& name
) const {
    if (!eos_) {
        throw std::runtime_error("EOS_py::eos_ not initialized");
    }

    double val = 0.0;
    auto status = (eos_->*func)(val);

    if (status != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error(
            "Error in " + name +
            " : status = " + std::to_string(static_cast<int>(status))
        );
    }

    return val;
}



double EOS_py::get_p_crit() const {
    return call_getter(&NEPTUNE::EOS::get_p_crit, "get_p_crit");
}

double EOS_py::get_h_crit() const {
    return call_getter(&NEPTUNE::EOS::get_h_crit, "get_h_crit");
}

double EOS_py::get_T_crit() const {
    return call_getter(&NEPTUNE::EOS::get_T_crit, "get_T_crit");
}

double EOS_py::get_rho_crit() const {
    return call_getter(&NEPTUNE::EOS::get_rho_crit, "get_rho_crit");
}

double EOS_py::get_p_min() const {
    return call_getter(&NEPTUNE::EOS::get_p_min, "get_p_min");
}

double EOS_py::get_p_max() const {
    return call_getter(&NEPTUNE::EOS::get_p_max, "get_p_max");
}

double EOS_py::get_h_max() const {
    return call_getter(&NEPTUNE::EOS::get_h_max, "get_h_max");
}

double EOS_py::get_h_min() const {
    return call_getter(&NEPTUNE::EOS::get_h_min, "get_h_min");
}

double EOS_py::get_T_max() const {
    return call_getter(&NEPTUNE::EOS::get_T_max, "get_T_max");
}

double EOS_py::get_T_min() const {
    return call_getter(&NEPTUNE::EOS::get_T_min, "get_T_min");
}

double EOS_py::get_rho_max() const {
    return call_getter(&NEPTUNE::EOS::get_rho_max, "get_rho_max");
}

double EOS_py::get_rho_min() const {
    return call_getter(&NEPTUNE::EOS::get_rho_min, "get_rho_min");
}

double EOS_py::get_mm() const {
    return call_getter(&NEPTUNE::EOS::get_mm, "get_mm");
}

double EOS_py::get_p() const {
    return call_getter(&NEPTUNE::EOS::get_p, "get_p");
}

EOS_Mixing_py::EOS_Mixing_py(
    std::vector<std::string> methods,
    std::vector<std::string> refs) : mixing_(nullptr)
{
    if (methods.empty())
    {
        throw std::runtime_error("EOS_Mixing_py: empty component list");
    }
    if (methods.size() != refs.size())
    {
        throw std::runtime_error("EOS_Mixing_py: methods and refs must have same size");
    }

    mixing_ = new NEPTUNE::EOS("EOS_Mixing");

    components_.reserve(methods.size());
    for (size_t i = 0; i < methods.size(); ++i)
    {
        components_.push_back(new NEPTUNE::EOS(methods[i].c_str(), refs[i].c_str()));
    }

    const int ierr = mixing_->set_components(
        components_.data(),
        static_cast<int>(components_.size()));

    if (ierr != 1)
    {
        throw std::runtime_error(
            "EOS_Mixing_py: set_components failed with error code " + std::to_string(ierr));
    }
}

EOS_Mixing_py::~EOS_Mixing_py() {
    for (size_t i = 0; i < components_.size(); ++i) {
        delete components_[i];
    }
    components_.clear();

    delete mixing_;
}

std::vector<std::vector<double>> EOS_Mixing_py::compute(
    std::vector<std::string> input_names,
    std::vector<std::vector<double>> input_values,
    std::vector<std::string> output_names
) {
    const size_t ninput = input_names.size();
    const size_t noutput = output_names.size();

    if (ninput == 0) {
        throw std::runtime_error("EOS_Mixing_py::compute: no input field provided");
    }
    if (noutput == 0) {
        throw std::runtime_error("EOS_Mixing_py::compute: no output field requested");
    }
    if (input_values.size() != ninput) {
        throw std::runtime_error("EOS_Mixing_py::compute: input_names and input_values must have same size");
    }

    const size_t n = input_values[0].size();
    if (n == 0) {
        throw std::runtime_error("EOS_Mixing_py::compute: empty input arrays");
    }

    for (size_t i = 0; i < ninput; ++i) {
        if (input_values[i].size() != n) {
            throw std::runtime_error("EOS_Mixing_py::compute: all input arrays must have same size");
        }
    }

    std::vector<NEPTUNE::ArrOfDouble> in_arrays(ninput);
    NEPTUNE::EOS_Fields inputs(static_cast<int>(ninput));

    for (size_t i = 0; i < ninput; ++i) {
        in_arrays[i].set_ptr(static_cast<int>(n), input_values[i].data());
        inputs[static_cast<int>(i)].init(
            input_names[i].c_str(),
            input_names[i].c_str(),
            in_arrays[i]
        );
    }

    std::vector<double> out_storage(noutput * n);
    std::vector<NEPTUNE::ArrOfDouble> out_arrays(noutput);
    NEPTUNE::EOS_Fields outputs(static_cast<int>(noutput));

    for (size_t i = 0; i < noutput; ++i) {
        out_arrays[i].set_ptr(static_cast<int>(n), &out_storage[i * n]);
        outputs[static_cast<int>(i)].init(
            output_names[i].c_str(),
            output_names[i].c_str(),
            out_arrays[i]
        );
    }

    std::vector<int> error(n, 0);
    NEPTUNE::EOS_Error_Field eos_error_field(static_cast<int>(n), error.data());

    NEPTUNE::EOS_Error worst = mixing_->compute(inputs, outputs, eos_error_field);
    if (worst != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error(
            "EOS_Mixing_py::compute: bad computation, worst error = " + std::to_string(static_cast<int>(worst))
        );
    }

    std::vector<std::vector<double>> result(n, std::vector<double>(noutput));
    for (size_t j = 0; j < n; ++j) {
        for (size_t i = 0; i < noutput; ++i) {
            result[j][i] = outputs[static_cast<int>(i)][static_cast<int>(j)];
        }
    }

    return result;
}

std::string EOS_Mixing_py::describe() const {
    std::ostringstream os;

    os << "** eos mixing" << "\n";
    os << "   * object     : EOS_Mixing_py" << "\n";

    if (!mixing_) {
        os << "   * state      : uninitialized" << "\n";
        return os.str();
    }

    os << "   * fluid      : " << mixing_->fluid_name() << "\n";
    os << "   * table      : " << mixing_->table_name() << "\n";
    os << "   * version    : " << mixing_->version_name() << "\n";
    os << "   * ncomp      : " << components_.size() << "\n";
    os << "   * components :" << "\n";

    for (size_t i = 0; i < components_.size(); ++i) {
        os << "      - [" << i << "] "
           << "fluid="   << components_[i]->fluid_name()
           << ", table=" << components_[i]->table_name()
           << ", version=" << components_[i]->version_name()
           << "\n";
    }

    return os.str();
}
