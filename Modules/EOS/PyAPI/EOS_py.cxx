#include "EOS_py.hxx"
#include <stdexcept>

EOS_py::EOS_py(const std::string& meth, const std::string& ref) {
    liquid = new NEPTUNE::EOS(meth.c_str(), ref.c_str());
}

EOS_py::~EOS_py() {
    delete liquid;
}

std::vector<std::vector<double>> EOS_py::compute(
    std::string str_P,
    std::string str_H,
    std::vector<double> tab_P,
    std::vector<double> tab_H,
    std::vector<std::string> str_out
) {
    unsigned int nb_calc = tab_P.size();
    unsigned int nb_prop = str_out.size();

    // Entrées
    NEPTUNE::EOS_Field inputP("Input 1", str_P.c_str(), nb_calc, &tab_P[0]);
    NEPTUNE::EOS_Field inputH("Input 2", str_H.c_str(), nb_calc, &tab_H[0]);

    // Sorties
    std::vector<double> outputs_vect(nb_prop * nb_calc);
    std::vector<NEPTUNE::ArrOfDouble> outputs_array(nb_prop);
    NEPTUNE::EOS_Fields outputs(nb_prop);

    for (unsigned int i = 0; i < nb_prop; i++) {
        outputs_array[i].set_ptr(nb_calc, &outputs_vect[i * nb_calc]);
        outputs[i].init(str_out[i].c_str(), str_out[i].c_str(), outputs_array[i]);
    }

    // Champ erreur
    std::vector<int> error(nb_calc, 0);
    NEPTUNE::EOS_Error_Field eos_error_field(nb_calc, &error[0]);

    // Calcul
    NEPTUNE::EOS_Error worst = liquid->compute(inputP, inputH, outputs, eos_error_field);
    if (worst != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error("Bad computation: Worst_error = " + std::to_string(worst));
    }

    // Conversion en std::vector<std::vector<double>>
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
    unsigned int nb_calc = tab_P.size();
    unsigned int nb_prop = str_out.size();

    // Entrées
    NEPTUNE::EOS_Field inputP("Input 1", str_P.c_str(), nb_calc, &tab_P[0]);

    // Sorties
    std::vector<double> outputs_vect(nb_prop * nb_calc);
    std::vector<NEPTUNE::ArrOfDouble> outputs_array(nb_prop);
    NEPTUNE::EOS_Fields outputs(nb_prop);

    for (unsigned int i = 0; i < nb_prop; i++) {
        outputs_array[i].set_ptr(nb_calc, &outputs_vect[i * nb_calc]);
        outputs[i].init(str_out[i].c_str(), str_out[i].c_str(), outputs_array[i]);
    }

    // Champ erreur
    std::vector<int> error(nb_calc, 0);
    NEPTUNE::EOS_Error_Field eos_error_field(nb_calc, &error[0]);

    // Calcul
    NEPTUNE::EOS_Error worst = liquid->compute(inputP, outputs, eos_error_field);
    if (worst != NEPTUNE::EOS_Error::good) {
        throw std::runtime_error("Bad computation: Worst_error = " + std::to_string(worst));
    }

    // Conversion en std::vector<std::vector<double>>
    std::vector<std::vector<double>> result(nb_calc, std::vector<double>(nb_prop));
    for (size_t j = 0; j < nb_calc; ++j) {
        for (size_t i = 0; i < nb_prop; ++i) {
            result[j][i] = outputs[i][j];
        }
    }

    return result;
}

