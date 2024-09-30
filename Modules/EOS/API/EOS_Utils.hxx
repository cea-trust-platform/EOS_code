#ifndef EOS_Utils_H
#define EOS_Utils_H

#include "Language/API/Strings.hxx"

namespace NEPTUNE
{
    char * packStrings(const NEPTUNE::Strings & s, int lString, long & size);
}

#endif /* EOS_Utils_H */