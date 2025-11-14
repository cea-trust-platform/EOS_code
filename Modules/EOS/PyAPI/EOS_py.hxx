#ifndef EOS_PY_HXX
#define EOS_PY_HXX

#include <vector>
#include <string>
#include "EOS/API/EOS.hxx"

class EOS_py {
public:
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

private:
    NEPTUNE::EOS* liquid;
};

#endif
