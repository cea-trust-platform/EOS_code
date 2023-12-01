%module EOS
%{
#include "Language/API/Language.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_i.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Fluid.hxx"
using namespace NEPTUNE;
%}

%include "../API/EOS_Field.hxx"
%include "../API/EOS_Error_Field.hxx"
%include "../API/EOS_Fields.hxx"
%include "../API/EOS_Fluid.hxx"
%include "../API/EOS.hxx"
