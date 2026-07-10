#include "EOS_py.hxx"
#include "Language/API/UObject.hxx"
#include "EOS/Src/EOS_Ipp/EOS_Ipp.hxx"
#include "EOS_IGen/API/EOS_IGen.hxx"

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

void EOS_py::set_interpolation_method(const std::string& mode) {
    if (!eos_) {
        throw std::runtime_error("EOS_py::eos_ not initialized");
    }
    NEPTUNE_EOS::EOS_Ipp* ipp = dynamic_cast<NEPTUNE_EOS::EOS_Ipp*>(&eos_->fluid());
    if (!ipp) {
        throw std::runtime_error(
            "set_interpolation_method: this EOS object does not wrap an EOS_Ipp interpolator"
        );
    }
    if (mode == "bilinear") {
        ipp->set_interpolation_method(NEPTUNE_EOS::EOS_Ipp::BILINEAR);
    } else if (mode == "bicubic") {
        ipp->set_interpolation_method(NEPTUNE_EOS::EOS_Ipp::BICUBIC);
    } else {
        throw std::runtime_error(
            "set_interpolation_method: mode must be 'bilinear' or 'bicubic', got '" + mode + "'"
        );
    }
}

std::string EOS_py::get_interpolation_method() const {
    if (!eos_) {
        throw std::runtime_error("EOS_py::eos_ not initialized");
    }
    NEPTUNE_EOS::EOS_Ipp* ipp = dynamic_cast<NEPTUNE_EOS::EOS_Ipp*>(&eos_->fluid());
    if (!ipp) {
        throw std::runtime_error(
            "get_interpolation_method: this EOS object does not wrap an EOS_Ipp interpolator"
        );
    }
    return (ipp->get_interpolation_method() == NEPTUNE_EOS::EOS_Ipp::BICUBIC) ? "bicubic" : "bilinear";
}

bool EOS_py::has_bicubic_first_derivative_data(const std::string& prop_name) const {
    if (!eos_) {
        throw std::runtime_error("EOS_py::eos_ not initialized");
    }
    NEPTUNE_EOS::EOS_Ipp* ipp = dynamic_cast<NEPTUNE_EOS::EOS_Ipp*>(&eos_->fluid());
    if (!ipp) {
        throw std::runtime_error(
            "has_bicubic_first_derivative_data: this EOS object does not wrap an EOS_Ipp interpolator"
        );
    }
    NEPTUNE::EOS_Property prop = NEPTUNE::gen_property_number(prop_name.c_str());
    return ipp->has_bicubic_first_derivative_data(prop);
}

bool EOS_py::has_bicubic_cross_derivative_data(const std::string& prop_name) const {
    if (!eos_) {
        throw std::runtime_error("EOS_py::eos_ not initialized");
    }
    NEPTUNE_EOS::EOS_Ipp* ipp = dynamic_cast<NEPTUNE_EOS::EOS_Ipp*>(&eos_->fluid());
    if (!ipp) {
        throw std::runtime_error(
            "has_bicubic_cross_derivative_data: this EOS object does not wrap an EOS_Ipp interpolator"
        );
    }
    NEPTUNE::EOS_Property prop = NEPTUNE::gen_property_number(prop_name.c_str());
    return ipp->has_bicubic_cross_derivative_data(prop);
}

EOS_IGen_py::EOS_IGen_py(const std::string& method, const std::string& reference)
    : igen_(nullptr), method_(method), reference_(reference),
      p_min_(0.0), p_max_(0.0), T_min_(0.0), T_max_(0.0),
      extremum_set_(false), mesh_built_(false)
{
    igen_ = new NEPTUNE_EOS_IGEN::EOS_IGen(method.c_str(), reference.c_str());
}

EOS_IGen_py::~EOS_IGen_py() {
    delete igen_;
}

void EOS_IGen_py::set_extremum(double p_min, double p_max, double T_min, double T_max) {
    if (p_min <= 0.0 || p_max <= p_min) {
        throw std::runtime_error(
            "EOS_IGen_py::set_extremum: need 0 < p_min < p_max (got p_min="
            + std::to_string(p_min) + ", p_max=" + std::to_string(p_max) + ")"
        );
    }
    if (T_max <= T_min) {
        throw std::runtime_error(
            "EOS_IGen_py::set_extremum: need T_min < T_max (got T_min="
            + std::to_string(T_min) + ", T_max=" + std::to_string(T_max) + ")"
        );
    }
    igen_->set_extremum(p_min, p_max, T_min, T_max);
    p_min_ = p_min;
    p_max_ = p_max;
    T_min_ = T_min;
    T_max_ = T_max;
    extremum_set_ = true;
}

void EOS_IGen_py::set_list_properties(std::vector<std::string> properties) {
    if (properties.empty()) {
        throw std::runtime_error(
            "EOS_IGen_py::set_list_properties: empty list -- omit the call entirely "
            "to keep EOS_IGen's full default property list"
        );
    }
    // EOS_IGen::set_list_propi takes a non-const reference: pass our local copy.
    igen_->set_list_propi(properties);
}

void EOS_IGen_py::make_mesh(int nb_mesh_p, int nb_mesh_h, int level_max) {
    if (!extremum_set_) {
        throw std::runtime_error("EOS_IGen_py::make_mesh: call set_extremum() first");
    }
    if (nb_mesh_p < 2 || nb_mesh_h < 2) {
        throw std::runtime_error(
            "EOS_IGen_py::make_mesh: nb_mesh_p and nb_mesh_h must be >= 2 (got "
            + std::to_string(nb_mesh_p) + ", " + std::to_string(nb_mesh_h) + ")"
        );
    }
    NEPTUNE::EOS_Error err = igen_->make_mesh(nb_mesh_p, nb_mesh_h, level_max);
    if (err != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error(
            "EOS_IGen_py::make_mesh: EOS_IGen::make_mesh failed (error code = "
            + std::to_string(static_cast<int>(err))
            + ") -- check that the domain corners are inside the fluid model's valid range"
        );
    }
    mesh_built_ = true;
}

void EOS_IGen_py::set_quality(const std::string& property, const std::string& type,
                               int is_abs, double limit_qi) {
    if (!mesh_built_) {
        throw std::runtime_error("EOS_IGen_py::set_quality: call make_mesh() first");
    }
    if (type != "centre" && type != "node") {
        throw std::runtime_error(
            "EOS_IGen_py::set_quality: type must be 'centre' or 'node', got '" + type + "'"
        );
    }
    igen_->set_quality(property.c_str(), type.c_str(), is_abs, limit_qi);
}

void EOS_IGen_py::make_global_refine() {
    if (!mesh_built_) {
        throw std::runtime_error("EOS_IGen_py::make_global_refine: call make_mesh() first");
    }
    NEPTUNE::EOS_Error err = igen_->make_global_refine();
    if (err != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error(
            "EOS_IGen_py::make_global_refine: EOS_IGen::make_global_refine failed "
            "(error code = " + std::to_string(static_cast<int>(err))
            + ") -- check that set_quality() was called with a property available in the base"
        );
    }
}

void EOS_IGen_py::make_local_refine(bool cont) {
    if (!mesh_built_) {
        throw std::runtime_error("EOS_IGen_py::make_local_refine: call make_mesh() first");
    }
    NEPTUNE::EOS_Error err = igen_->make_local_refine(cont);
    if (err != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error(
            "EOS_IGen_py::make_local_refine: EOS_IGen::make_local_refine failed "
            "(error code = " + std::to_string(static_cast<int>(err))
            + ") -- check that set_quality() was called with a property available in the base"
        );
    }
}

void EOS_IGen_py::write_med(const std::string& file_name) {
    if (!mesh_built_) {
        throw std::runtime_error("EOS_IGen_py::write_med: call make_mesh() first");
    }
    NEPTUNE::AString name(file_name.c_str());
    igen_->set_file_med_name(name);
    NEPTUNE::EOS_Error err = igen_->write_med();
    if (err != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error(
            "EOS_IGen_py::write_med: EOS_IGen::write_med failed (error code = "
            + std::to_string(static_cast<int>(err))
            + ") -- check that USER_EOS_DATA is set and writable"
        );
    }
}

std::string EOS_IGen_py::describe() const {
    std::ostringstream oss;
    oss << "** eos igen (mesh generator) **" << "\n";
    oss << "   * method    : " << method_ << "\n";
    oss << "   * reference : " << reference_ << "\n";
    if (extremum_set_) {
        oss << "   * domain    : p in [" << p_min_ << ", " << p_max_ << "] Pa, "
            << "T in [" << T_min_ << ", " << T_max_ << "] K" << "\n";
    } else {
        oss << "   * domain    : not set (call set_extremum)" << "\n";
    }
    oss << "   * mesh      : " << (mesh_built_ ? "built" : "not built yet") << "\n";
    return oss.str();
}

double EOS_IGen_py::get_p_min() const { return p_min_; }
double EOS_IGen_py::get_p_max() const { return p_max_; }
double EOS_IGen_py::get_T_min() const { return T_min_; }
double EOS_IGen_py::get_T_max() const { return T_max_; }


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
