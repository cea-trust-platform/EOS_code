%module eos_py

%{
#include "EOS_py.hxx"
%}

/* Support automatique pour std::string et std::vector */
%include "std_string.i"
%include "std_vector.i"

namespace std {
    %template(DoubleVector) vector<double>;
    %template(StringVector) vector<string>;
    %template(DoubleVectorVector) vector<vector<double>>;
}

/* Inclure la classe à exposer */
%include "EOS_py.hxx"