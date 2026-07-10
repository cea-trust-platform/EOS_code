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
# Test de l'API Python du melange EOS_Mixing (SWIG, module eos_py, classe
# EOS_Mixing_py).
#
# Reprend le scenario du notebook
# EOS/api_python_interpolateur/eos_py_demo_mixing.ipynb (melange
# eau vapeur / air incondensable via EOS_Cathare2) mais avec de vraies
# assertions plutot qu'un simple affichage. Plutot que de figer les valeurs
# numeriques du notebook (dependantes de la version du plugin Cathare2), ce
# test verifie des invariants physiques robustes :
#   - coherence aller-retour (p,T) -> h -> (p,h) -> T
#   - monotonie de rho et Tsat avec la pression
# et l'ensemble des chemins d'erreur documentes dans EOS_py.cxx
# (EOS_Mixing_py::EOS_Mixing_py / EOS_Mixing_py::compute).

import sys

import eos_py


METHODS = ["EOS_Cathare2", "EOS_Cathare2"]
REFS = ["WaterVapor", "AirIncondensable"]

ROUND_TRIP_REL_TOL = 1.e-6

failures = []


def check(condition, message):
    if not condition:
        print("FAILED:", message)
        failures.append(message)
    else:
        print("OK:", message)


def expect_runtime_error(label, fn):
    try:
        fn()
        check(False, "{} doit lever une RuntimeError".format(label))
    except RuntimeError:
        check(True, "{} leve bien une RuntimeError".format(label))


def main():
    # --- 1. Construction : cas d'erreur ------------------------------------
    expect_runtime_error(
        "EOS_Mixing_py([], [])",
        lambda: eos_py.EOS_Mixing_py([], []),
    )
    expect_runtime_error(
        "EOS_Mixing_py avec methods/refs de tailles differentes",
        lambda: eos_py.EOS_Mixing_py(["EOS_Cathare2"], REFS),
    )

    # --- 2. Construction valide + describe() --------------------------------
    mix = eos_py.EOS_Mixing_py(METHODS, REFS)
    description = mix.describe()
    check("EOS_Mixing_py" in description,
          "describe() doit mentionner EOS_Mixing_py")
    check("ncomp      : 2" in description,
          "describe() doit rapporter 2 composants")
    check("Water" in description and "Air" in description,
          "describe() doit lister les fluides des composants (Water, Air)")

    # --- 3. Calcul direct (p,T,c_0,c_1) -> (h,rho,cp) -----------------------
    input_names = ["p", "T", "c_0", "c_1"]
    input_values = [[1.0e5, 2.0e5], [393.15, 393.15], [0.5, 0.5], [0.5, 0.5]]
    output_names = ["h", "rho", "cp"]
    results = mix.compute(input_names, input_values, output_names)

    check(len(results) == 2 and all(len(row) == 3 for row in results),
          "compute() doit renvoyer 2 lignes de 3 valeurs (h, rho, cp)")

    (h0, rho0, cp0), (h1, rho1, cp1) = results
    check(h0 > 0. and h1 > 0., "l'enthalpie du melange doit etre positive")
    check(rho0 > 0. and rho1 > 0., "la masse volumique du melange doit etre positive")
    check(cp0 > 0. and cp1 > 0., "le cp du melange doit etre positif")
    check(rho1 > rho0,
          "a T fixee, rho doit augmenter avec p (rho(2e5)={:.6g} vs rho(1e5)={:.6g})".format(rho1, rho0))

    # --- 4. Coherence aller-retour : (p,h,c_0,c_1) -> T doit redonner T=393.15
    rt_input_names = ["p", "h", "c_0", "c_1"]
    rt_input_values = [[1.0e5, 2.0e5], [h0, h1], [0.5, 0.5], [0.5, 0.5]]
    rt_results = mix.compute(rt_input_names, rt_input_values, ["T"])

    for (T_rt,), T_orig in zip(rt_results, (393.15, 393.15)):
        rel_err = abs(T_rt - T_orig) / abs(T_orig)
        check(rel_err < ROUND_TRIP_REL_TOL,
              "aller-retour (p,T)->h->(p,h)->T incoherent (T={:.9g}, attendu {:.9g}, erreur relative={:.3e})".format(
                  T_rt, T_orig, rel_err))

    # --- 5. Saturation : Tsat doit croitre avec p ---------------------------
    sat_input_names = ["p", "c_0", "c_1"]
    sat_pressures = [1.0e5, 2.0e5, 3.0e5, 4.0e5]
    sat_input_values = [sat_pressures, [0.5] * 4, [0.5] * 4]
    sat_results = mix.compute(sat_input_names, sat_input_values, ["Tsat"])
    tsat = [row[0] for row in sat_results]

    check(all(t2 > t1 for t1, t2 in zip(tsat, tsat[1:])),
          "Tsat doit croitre strictement avec la pression (valeurs={})".format(tsat))

    # --- 6. compute() : cas d'erreur -----------------------------------------
    expect_runtime_error(
        "compute() sans champ d'entree",
        lambda: mix.compute([], [], ["h"]),
    )
    expect_runtime_error(
        "compute() sans grandeur de sortie",
        lambda: mix.compute(["p"], [[1.0e5]], []),
    )
    expect_runtime_error(
        "compute() avec input_names/input_values de tailles differentes",
        lambda: mix.compute(["p", "T"], [[1.0e5]], ["h"]),
    )
    expect_runtime_error(
        "compute() avec un tableau d'entree vide",
        lambda: mix.compute(["p"], [[]], ["h"]),
    )
    expect_runtime_error(
        "compute() avec des tableaux d'entree de tailles incoherentes",
        lambda: mix.compute(["p", "T"], [[1.0e5, 2.0e5], [393.15]], ["h"]),
    )

    if failures:
        print("\n{} verification(s) en echec".format(len(failures)))
        sys.exit(1)

    print("\nToutes les verifications ont reussi.")


if __name__ == "__main__":
    main()
