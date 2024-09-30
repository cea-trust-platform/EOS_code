#include "EOS_Utils.hxx"

#include <cstring>

using NEPTUNE::Strings ;

namespace NEPTUNE
{
    char * packStrings(const NEPTUNE::Strings & s, int lString, long & size)
    {
        assert(lString > 0);

        int iS, nS = s.size();
        size_t lS = lString;
        size = nS * lString;

        char * t = new char[size+1];
        std::memset(t, ' ', size);
        t[size] = '\0';

        for (iS = 0; iS < nS; iS++)
        {
            size_t l = strlen(s[iS].aschar());
            if (lS < l) l = lS;

            memcpy(t + iS*lString, s[iS].aschar(), l);
        }
        return t;
    }
}