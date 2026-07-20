#****************************************************************************
# Copyright (c) 2026, CEA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************
#
# Test de l'API Python de l'interpolateur EOS_Ipp / EOS_IGen (SWIG, module
# eos_py), en particulier le mode d'interpolation bicubique introduit par
# EOS_46000.
#
# Reprend le scenario du notebook
# EOS/api_python_interpolateur/eos_py_demo_interpolateur.ipynb (generation
# d'un maillage (p,h) avec EOS_IGen_py puis interpolation bilineaire /
# bicubique avec EOS_Ipp via EOS_py) et les tolerances du test C++ equivalent
# Modules/EOS/Tests/C++/main_ipp_bicubic.cxx, mais en verifiant chaque etape
# avec de vraies assertions plutot qu'en imprimant les resultats.
#
# A la difference du fluide reel (compute("p","t",...) accepte (p,h) ou
# (p,T)), EOS_Ipp n'accepte QUE des points (p,h) en entree et ne connait que
# les proprietes explicitement stockees dans la base .med au moment de sa
# generation (cf. documentation_api_python_interpolateur.ipynb, section
# "pieges") : toute autre combinaison ne leve pas d'exception Python propre
# et fait planter le processus. Ce test ne s'ecarte donc jamais de ces deux
# contraintes.

import sys

import eos_py


METHOD, REFERENCE = "EOS_Refprop9", "WaterLiquid"
PMIN, PMAX = 1.0e7, 2.0e7   # Pa
TMIN, TMAX = 300.0, 500.0   # K
PROPERTIES = ["T", "d_T_d_p_h", "d_T_d_h_p", "d2_T_d_p_d_h"]
MESH_NAME = "test_eos_ipp_bicubic_py_mesh"

# Marge large (mesh 8x8 sans raffinement) : cf. main_ipp_bicubic.cxx pour les
# memes ordres de grandeur sur un maillage comparable.
REL_TOL_EXACT = 1.e-6
REL_TOL_BILINEAR = 0.05
REL_TOL_BICUBIC = 0.02

failures = []


def check(condition, message):
    if not condition:
        print("FAILED:", message)
        failures.append(message)
    else:
        print("OK:", message)


def main():
    # --- Fluide reel, verite terrain pour les comparaisons ci-dessous ---
    fluide = eos_py.EOS_py(METHOD, REFERENCE)

    # --- 1. Generation d'un maillage (p,h) avec EOS_IGen_py -----------------
    # set_extremum -> make_mesh -> set_list_properties -> write_med, cf.
    # eos_py_demo_interpolateur.ipynb section 1.1 (maillage regulier).
    igen = eos_py.EOS_IGen_py(METHOD, REFERENCE)
    igen.set_extremum(PMIN, PMAX, TMIN, TMAX)
    igen.make_mesh(8, 8)   # level_max=-1 par defaut : pas de raffinement
    igen.set_list_properties(PROPERTIES)
    igen.write_med(MESH_NAME)

    ipp = eos_py.EOS_py("EOS_Ipp", MESH_NAME)

    # --- 2. Methode d'interpolation par defaut = bicubique -------------------
    check(ipp.get_interpolation_method() == "bicubic",
          "get_interpolation_method() par defaut doit valoir 'bicubic' (obtenu '{}')".format(
              ipp.get_interpolation_method()))

    # --- 3. Presence des donnees necessaires au mode bicubique --------------
    check(ipp.has_bicubic_first_derivative_data("T"),
          "la base generee doit exposer les derivees premieres de T")
    check(ipp.has_bicubic_cross_derivative_data("T"),
          "la base generee doit exposer la derivee croisee stockee de T (d2_T_d_p_d_h)")

    # --- 4. Nom de methode invalide : doit lever une exception Python -------
    try:
        ipp.set_interpolation_method("bogus")
        check(False, "set_interpolation_method('bogus') doit lever une RuntimeError")
    except RuntimeError:
        check(True, "set_interpolation_method('bogus') leve bien une RuntimeError")

    # --- 5. Point exact a un noeud du maillage : bilineaire et bicubique ----
    # doivent tous les deux reproduire la valeur reelle du fluide.
    p_min, p_max = ipp.get_p_min(), ipp.get_p_max()
    h_min, h_max = ipp.get_h_min(), ipp.get_h_max()

    T_ref_corner = fluide.compute("p", "h", [p_min], [h_min], ["T"])[0][0]
    for method in ("bilinear", "bicubic"):
        ipp.set_interpolation_method(method)
        T_corner = ipp.compute("p", "h", [p_min], [h_min], ["T"])[0][0]
        rel_err = abs(T_corner - T_ref_corner) / abs(T_ref_corner)
        check(rel_err < REL_TOL_EXACT,
              "{}: le noeud du maillage doit reproduire la base exactement (erreur relative={:.3e})".format(
                  method, rel_err))

    # --- 6. Points interieurs : precision vs. fluide reel, bicubique >= bilineaire
    p_test = [p_min + (p_max - p_min) * f for f in (0.2, 0.4, 0.5, 0.6, 0.8)]
    h_test = [h_min + (h_max - h_min) * f for f in (0.2, 0.4, 0.5, 0.6, 0.8)]
    pp = [p for p in p_test for _ in h_test]
    hh = [h for _ in p_test for h in h_test]

    T_ref = [row[0] for row in fluide.compute("p", "h", pp, hh, ["T"])]

    max_rel_err = {}
    for method in ("bilinear", "bicubic"):
        ipp.set_interpolation_method(method)
        T_interp = [row[0] for row in ipp.compute("p", "h", pp, hh, ["T"])]
        max_rel_err[method] = max(
            abs(t_i - t_r) / abs(t_r) for t_i, t_r in zip(T_interp, T_ref)
        )

    print("erreur relative max sur T : bilinear={:.3e}  bicubic={:.3e}".format(
        max_rel_err["bilinear"], max_rel_err["bicubic"]))

    check(max_rel_err["bilinear"] < REL_TOL_BILINEAR,
          "erreur bilineaire hors tolerance ({:.3e})".format(max_rel_err["bilinear"]))
    check(max_rel_err["bicubic"] < REL_TOL_BICUBIC,
          "erreur bicubique hors tolerance ({:.3e})".format(max_rel_err["bicubic"]))
    check(max_rel_err["bicubic"] <= max_rel_err["bilinear"] * 1.1 + 1.e-12,
          "le mode bicubique ne doit pas etre moins precis que le bilineaire (bicubic={:.3e}, bilinear={:.3e})".format(
              max_rel_err["bicubic"], max_rel_err["bilinear"]))

    # --- 7. Un seul point vs une liste de points : meme valeur au meme (p,h)
    ipp.set_interpolation_method("bicubic")
    p0, h0 = pp[0], hh[0]
    T_single = ipp.compute("p", "h", [p0], [h0], ["T"])[0][0]
    T_list_first = ipp.compute("p", "h", pp, hh, ["T"])[0][0]
    check(T_single == T_list_first,
          "l'evaluation sur un seul point doit coincider avec le meme point pris dans une liste")

    # --- 8. Point hors domaine : erreur Python propre, pas de crash ---------
    p_oob = p_max + (p_max - p_min)
    for method in ("bilinear", "bicubic"):
        ipp.set_interpolation_method(method)
        try:
            ipp.compute("p", "h", [p_oob], [h_min], ["T"])
            check(False, "{}: un point hors domaine doit lever une RuntimeError".format(method))
        except RuntimeError:
            check(True, "{}: un point hors domaine leve bien une RuntimeError".format(method))

    if failures:
        print("\n{} verification(s) en echec sur {}".format(len(failures), MESH_NAME))
        sys.exit(1)

    print("\nToutes les verifications ont reussi.")


if __name__ == "__main__":
    main()
