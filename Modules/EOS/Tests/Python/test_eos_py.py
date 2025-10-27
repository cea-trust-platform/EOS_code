#****************************************************************************
# Copyright (c) 2015 - 2016, CEA
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
import  eos_py 
 


# Fonction pour afficher les resultats
           
def affiche(tab_P, tab_H, result, outputs_string, max_columns=5):
    """
    Affiche le contenu de result en colonnes distinctes sans déborder, en affichant par blocs.
    
    :param tab_P: Liste des pressions
    :param tab_H: Liste des enthalpies
    :param result: Liste des résultats obtenus
    :param outputs_string: Liste des noms des propriétés
    :param max_columns: Nombre maximal de colonnes affichées à la fois
    """
    num_props = len(outputs_string)
    for start in range(0, num_props, max_columns):
        end = min(start + max_columns, num_props)
        props_subset = outputs_string[start:end]
        
        print(f"{'P':<15}{'H/T':<15}" + "".join([f"{prop:<15}" for prop in props_subset]))
        print("-" * (30 + 15 * len(props_subset)))
        
        for i, (P, H) in enumerate(zip(tab_P, tab_H)):
            values = "".join([f"{result[i][j]:<15.6g}" for j in range(start, end)])
            print(f"{P:<15}{H:<15}{values}")
        
        print("\n")  # Ajoute une séparation entre les blocs
        
        

def affiche_sat(tab_P, result, outputs_string, max_columns=5):
    """
    Affiche le contenu de result en colonnes distinctes sans déborder, en affichant par blocs,
    en conservant uniquement P.
    
    :param tab_P: Liste des pressions
    :param result: Liste des résultats obtenus
    :param outputs_string: Liste des noms des propriétés
    :param max_columns: Nombre maximal de colonnes affichées à la fois
    """
    num_props = len(outputs_string)
    for start in range(0, num_props, max_columns):
        end = min(start + max_columns, num_props)
        props_subset = outputs_string[start:end]
        
        print(f"{'P':<15}" + "".join([f"{prop:<15}" for prop in props_subset]))
        print("-" * (15 + 15 * len(props_subset)))
        
        for i, P in enumerate(tab_P):
            values = "".join([f"{result[i][j]:<15.6g}" for j in range(start, end)])
            print(f"{P:<15}{values}")
        
        print("\n")  # Ajoute une séparation entre les blocs





# Création de l'objet EOS_py
eos = eos_py.EOS_py("Cathare2", "WaterLiquid")
#eosliq = eos_py.EOS_py("Cathare2", "WaterLiquid")
#eosvap = eos_py.EOS_py("Cathare2", "WaterVapor")


# Définition des points de calculs
tab_P=[100000.0, 200000.0, 300000.0] #en Pa
tab_H=[400000.0, 500000.0, 600000.0] #en 
tab_T=[300, 350, 400] #en Kelvin 

# Définition des propriétes d'intérets
"""
outputs_string = [
    "beta", "cp", "cv", "d_beta_d_h_p", "d_beta_d_p_h", "d_beta_d_T_p", "d_cp_d_h_p", "d_cp_d_p_h",
    "d_cp_d_p_T", "d_cp_d_T_p", "d_cv_d_h_p", "d_cv_d_p_h", "d_cv_d_p_T", "d_cv_d_T_p",
    "d_f_d_h_p", "d_f_d_p_h", "d_f_d_T_p", "d_gamma_d_T_p", "d_g_d_h_p", "d_g_d_p_h", "d_g_d_T_p",
    "d_h_d_p_T", "d_h_d_T_p", "d_lambda_d_h_p", "d_lambda_d_p_h", "d_lambda_d_p_T",
    "d_lambda_d_T_p", "d_mu_d_h_p", "d_mu_d_p_h", "d_mu_d_p_T", "d_mu_d_T_p", "d_pr_d_h_p",
    "d_pr_d_p_h", "d_pr_d_T_p", "d_rho_d_h_p", "d_rho_d_p_h", "d_rho_d_p_T", "d_rho_d_T_p",
    "d_s_d_h_p", "d_s_d_p_h", "d_s_d_p_T", "d_s_d_T_p", "d_sigma_d_h_p", "d_sigma_d_p_h",
    "d_sigma_d_T_p", "d_T_d_h_p", "d_T_d_p_h", "d_u_d_h_p", "d_u_d_p_h", "d_u_d_T_p", "d_w_d_h_p",
    "d_w_d_p_h", "d_w_d_p_T", "d_w_d_T_p", "f", "g", "lambda", "mu", "pr", "rho", "s", "sigma",
    "T", "u", "w"
]
"""  # Pas compatible avec tout 

outputs_string = ["beta","T","cp","d_beta_d_h_p", "d_beta_d_p_h","d_cp_d_h_p", "d_cp_d_p_h","d_cp_d_p_T","d_cp_d_T_p","d_mu_d_h_p", "d_mu_d_p_h","d_rho_d_h_p", "d_rho_d_p_h", "d_rho_d_p_T", "d_rho_d_T_p","d_T_d_h_p", "d_T_d_p_h","mu","rho"] #Compatible avec Cathare2 WL

outputs_string_T = ["beta","h","cp","d_beta_d_h_p", "d_beta_d_p_h","d_cp_d_h_p", "d_cp_d_p_h","d_cp_d_p_T","d_cp_d_T_p","d_mu_d_h_p", "d_mu_d_p_h","d_rho_d_h_p", "d_rho_d_p_h", "d_rho_d_p_T", "d_rho_d_T_p","d_T_d_h_p", "d_T_d_p_h","mu","rho"]

tab_P = list(tab_P)
tab_H = list(tab_H)
tab_T = list(tab_T)
outputs_string = list(outputs_string)
outputs_string_T = list(outputs_string_T)

# Utilisation de compute
result1 = eos.compute("p", "h",tab_P,tab_H,outputs_string)
result3 = eos.compute("p", "t",tab_P,tab_T,outputs_string_T)


print("Résultat du calcul :")


affiche(tab_P, tab_H, result1, outputs_string)
affiche(tab_P, tab_T, result3, outputs_string)
# Utilisation de compute_sat

outputs_string = ["cplsat",
"cpvsat",
"d2tsatdpdp",
"dcplsatdp",
"dcpvsatdp",
"dhlsatdp",
"dhvsatdp",
"drholsatdp",
"drhovsatdp",
"dtsatdp",
"hlsat",
"hvsat",
"rholsat",
"rhovsat",
"tsat"] # Compatible avec C2WL


#outputs_string= ["hlsat","Tsat"]

tab_P = list(tab_P)
tab_H = list(tab_H)
tab_T = list(tab_T)
outputs_string = list(outputs_string)
outputs_string_T = list(outputs_string_T)
result2 = eos.compute_sat("p",tab_P,outputs_string)

print("Résultat du calcul à saturation :")
affiche_sat(tab_P, result2, outputs_string)

