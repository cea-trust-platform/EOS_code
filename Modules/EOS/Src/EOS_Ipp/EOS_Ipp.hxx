/****************************************************************************
 * Copyright (c) 2023, CEA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

//
#ifndef EOS_IPP_HXX_
#define EOS_IPP_HXX_

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Error.hxx"             // ajout M.F.
#include "EOS/API/EOS_Std_Error_Handler.hxx" // ajout M.F.
#include "Language/API/Language.hxx"
#include "EOS_IGen/Src/EOS_Med.hxx"
#include <vector>
#include <string>
using std::vector;

using namespace NEPTUNE;

namespace NEPTUNE_EOS
{
       class EOS_Ipp_TileCache; // Src/EOS_Ipp_TileCache.hxx: lazy-loaded (p,h) tile cache backing the
                                 // "tiled database" (streaming) mode of this class, cf. init()/tile_cache_.

       //! Interpolation data of one mesh cell (4 corners) or one saturation /
       //! limit segment (2 endpoints), as a plain block of doubles.
       //!
       //! Rows, for a 2D (p,h) cell:
       //!   [0] = p           [1] = h
       //!   [2] = f  (the interpolated property)
       //!   [3] = d f/dp |h   [4] = d f/dh |p     (bicubic only)
       //!   [5] = d2 f/dp.dh  (bicubic only, when the database stores it)
       //! For a 1D saturation/limit segment only [0] = p and [1] = f are used,
       //! with 2 valid entries instead of 4.
       //!
       //! This used to be an EOS_Fields of ArrOfDouble, rebuilt on every single
       //! compute_* call. Those are Language NumberedObjects: each construction
       //! and destruction allocates, and registers/unregisters itself in the
       //! global OBJECTSHANDLING::Objects registry behind a process-wide mutex.
       //! Profiling the scalar hot path put ~33% of its time in malloc/free and
       //! ~24% in that registry, against ~4% in the interpolation itself -- and
       //! the registry's own reallocation races made the whole path unusable
       //! from several threads at once. None of the interpolators ever needed
       //! more than indexed doubles, so they now take this instead.
       struct EOS_Ipp_CellData
       {
              enum { NB_ROWS = 6, NB_POINTS = 4 };
              double v[NB_ROWS][NB_POINTS];

              double *operator[](int row) { return v[row]; }
              const double *operator[](int row) const { return v[row]; }
       };

       class EOS_Ipp : public EOS_Fluid
       {
              static const AString tablename;
              friend class EOS_Ipp_TileCache; // needs compute_prop_ph/compute_prop_p on each tile's EOS_Ipp

       public:
              //! Interpolation method used on the 2D (p,h) mesh for physical properties.
              //! BILINEAR is the historical/default behaviour, preserved for compatibility.
              enum Interpolation_Method
              {
                     BILINEAR = 0,
                     BICUBIC = 1
              };

              virtual const AString &table_name() const;
              mutable bool switch_model;         // If true: override the compute functions when the calculation is not ok
              mutable bool switch_comp_sat_;     // If true: override the compute functions when the calculation is not ok
              mutable bool swch_calc_deriv_fld_; // If true: compute d_lambda_d_h_p using the fluid's own method
              //! Per-point interpolated (r1_val) and reference-model (r2_val)
              //! values, filled by compute_() so it can report the gap between
              //! them. Empty until compute_() is called and sized by it to the
              //! batch it received: they used to be born 20x30 and indexed
              //! [property][point] unchecked, which any batch of more than 30
              //! points overran.
              mutable std::vector<std::vector<double>> r1_val;
              mutable std::vector<std::vector<double>> r2_val;
              void resize_debug_grids(int nb_prop, int nb_pts) const;
              EOS *obj_fluid = nullptr;
              EOS_Ipp();
              virtual ~EOS_Ipp();

              //! to initialize an implementation of EOS_Ipp
              virtual int init(const Strings &);
              //! to initialize an implementation of EOS_Ipp with supplementary parameters
              virtual int init(const Strings &, const Strings &);

              //! Loads a single, standalone .med file at an exact path, bypassing the
              //! {DATA}/EOS_Ipp/ directory convention used by init(const Strings&).
              //! This factors out the historical, eager, whole-database loading body of
              //! init(const Strings&) so it can also be used by EOS_Ipp_TileCache to load
              //! one tile of a tiled database (cf. init()'s ".eosmm" manifest detection).
              //! Public because EOS_Ipp_TileCache constructs plain EOS_Ipp instances (one
              //! per tile) rather than being a subclass.
              //! properties, when non-empty, restricts the loading to those fields
              //! (same selection as init(const Strings&, const Strings&)). Reading a
              //! .med field is what dominates a tile load, so a tiled database opened
              //! for a handful of properties both loads and occupies proportionally
              //! less.
              EOS_Error load_from_med_path(const AString &full_med_path,
                                           const Strings &properties = Strings());

              //! Select the interpolation method to use on the 2D (p,h) mesh.
              //! Has no effect on the 1D saturation/limit curves (always linear).
              void set_interpolation_method(Interpolation_Method method);
              Interpolation_Method get_interpolation_method() const;

              //! Approximate resident size of this instance's loaded database, in
              //! bytes: the mesh nodes, the property values, the connectivity and
              //! the per-cell error fields. Used to give the tile cache a budget in
              //! bytes rather than in tiles -- a tile count says nothing about how
              //! much memory a database will occupy, since that depends entirely on
              //! how finely each tile was meshed.
              std::size_t approximate_footprint_bytes() const;

              //! Tiled mode only (null tile_cache_ otherwise): how many tiles are
              //! currently resident, how many were loaded from disk since init, how
              //! many were evicted, and the resident bytes. Returns false when this
              //! instance is not a tiled database. Loads far above the number of
              //! distinct tiles a run touches means the cache is thrashing and the
              //! budget is too small.
              bool get_tile_cache_stats(std::size_t &nb_resident, std::size_t &nb_loads,
                                        std::size_t &nb_evictions, std::size_t &resident_bytes) const;

              //! True if the 2D (p,h) field of prop was loaded from the database.
              //! False both for a property the database does not carry and for one
              //! left out when the database was opened for a subset of properties.
              bool has_ph_property(EOS_Property prop) const;

              //! True if prop is a base 2D property with both first-derivative fields
              //! (d_prop_d_p_h, d_prop_d_h_p) loaded from the current database -- the
              //! minimum required for BICUBIC; otherwise BICUBIC falls back to bilinear.
              bool has_bicubic_first_derivative_data(EOS_Property prop) const;
              //! True if, in addition, the stored cross derivative (d2_prop_d_p_d_h) is
              //! loaded -- used in place of the local twist approximation when available.
              bool has_bicubic_cross_derivative_data(EOS_Property prop) const;

              //! Error handling methods
              void describe_error(const EOS_Internal_Error error, AString &description) const;

              static const EOS_Internal_Error OUT_OF_BOUNDS;
              static const EOS_Internal_Error INVERT_h_pT;
              static const EOS_Internal_Error PROP_NOT_IN_DB;
              static const EOS_Internal_Error MODEL_NOT_INIT;

              //! critical T
              virtual EOS_Internal_Error get_T_crit(double &) const;
              //! critical p
              virtual EOS_Internal_Error get_p_crit(double &) const;
              //! critical h
              virtual EOS_Internal_Error get_h_crit(double &) const;

              //! limits values
              virtual EOS_Internal_Error get_h_min(double &) const;
              virtual EOS_Internal_Error get_h_max(double &) const;
              virtual EOS_Internal_Error get_T_min(double &) const;
              virtual EOS_Internal_Error get_T_max(double &) const;
              virtual EOS_Internal_Error get_p_min(double &) const;
              virtual EOS_Internal_Error get_p_max(double &) const;
              virtual EOS_Internal_Error get_error_Ipp(double &) const;

              // Debugage de REFPROP10
              // Molar mass (kg/mol)
              virtual EOS_Internal_Error get_mm(double &) const;

              virtual EOS_Internal_Error get_nbcell(int &) const;

              //
              //  Other methods
              //
              //! see Language
              virtual ostream &print_On(ostream &stream = cout) const;
              //! see Language
              virtual istream &read_On(istream &stream = cin);
              //! see Language
              virtual const Type_Info &get_Type_Info() const;

              // Compute interpolation error
              virtual EOS_Internal_Error compute_Ipp_error(double &error_tot, double *&error_cells, EOS_Property prop);
              virtual EOS_Internal_Error compute_Ipp_sat_error(double &error_tot, double *&error_cells, EOS_Property prop);
              //! h(p,T)
              virtual EOS_Internal_Error compute_h_pT(double p, double T, double &h) const;
              // virtual EOS_Internal_Error compute_d_h_d_T_pT(double p, double T, double& h) const;

              //! T(p,h)
              virtual EOS_Internal_Error compute_T_ph(double p, double h, double &) const;
              //! d(T)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_T_d_p_h_ph(double p, double h, double &) const;
              //! d(T)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_T_d_h_p_ph(double p, double h, double &) const;
              //! rho(p,h)
              virtual EOS_Internal_Error compute_rho_ph(double p, double h, double &) const;
              //! rho(p,T)
              virtual EOS_Internal_Error compute_rho_pT(double p, double T, double &) const;
              //! d(rho)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_rho_d_p_h_ph(double p, double h, double &) const;
              //! d(rho)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_rho_d_h_p_ph(double p, double h, double &) const;
              //! u(p,h)
              virtual EOS_Internal_Error compute_u_ph(double p, double h, double &) const;
              //! u(p,T)
              virtual EOS_Internal_Error compute_u_pT(double p, double T, double &) const;
              //! d(u)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_u_d_p_h_ph(double p, double h, double &) const;
              //! d(u)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_u_d_h_p_ph(double p, double h, double &) const;
              //! s(p,h)
              virtual EOS_Internal_Error compute_s_ph(double p, double h, double &) const;
              //! s(p,T)
              virtual EOS_Internal_Error compute_s_pT(double p, double T, double &) const;
              //! d(s)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_s_d_p_h_ph(double p, double h, double &) const;
              //! d(s)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_s_d_h_p_ph(double p, double h, double &) const;
              //! mu(p,h)
              virtual EOS_Internal_Error compute_mu_ph(double p, double h, double &) const;
              //! mu(p,T)
              virtual EOS_Internal_Error compute_mu_pT(double p, double T, double &) const;
              //! d(mu)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_mu_d_p_h_ph(double p, double h, double &) const;
              //! d(mu)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_mu_d_h_p_ph(double p, double h, double &) const;
              //! lambda(p,h)
              virtual EOS_Internal_Error compute_lambda_ph(double p, double h, double &) const;
              //! lambda(p,T)
              virtual EOS_Internal_Error compute_lambda_pT(double p, double T, double &) const;
              //! d(lambda)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_lambda_d_p_h_ph(double p, double h, double &) const;
              //! d(h)/dp      at constant specific temp
              virtual EOS_Internal_Error compute_d_h_d_p_T_pT(double p, double T, double &r,
                                                              double c_0, double c_1, double c_2, double c_3, double c_4) const;
              virtual EOS_Internal_Error compute_d_h_d_p_T_pT(double p, double T, double &r) const;
              virtual EOS_Internal_Error compute_d_h_d_T_p_pT(double p, double T, double &r) const;
              //! d(lambda)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_lambda_d_h_p_ph(double p, double h, double &) const;
              //! cp(p,h)
              virtual EOS_Internal_Error compute_cp_ph(double p, double h, double &) const;
              //! cp(p,T)
              virtual EOS_Internal_Error compute_cp_pT(double p, double T, double &) const;
              //! d(cp)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_cp_d_p_h_ph(double p, double h, double &) const;
              //! d(cp)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_cp_d_h_p_ph(double p, double h, double &) const;
              //! sigma(p,h)
              virtual EOS_Internal_Error compute_sigma_ph(double p, double h, double &) const;
              //! sigma(p,T)
              virtual EOS_Internal_Error compute_sigma_pT(double p, double T, double &) const;
              //! d(sigma)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_sigma_d_p_h_ph(double p, double h, double &) const;
              //! d(sigma)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_sigma_d_h_p_ph(double p, double h, double &) const;
              //! w(p,h)
              virtual EOS_Internal_Error compute_w_ph(double p, double h, double &) const;
              //! w(p,T)
              virtual EOS_Internal_Error compute_w_pT(double p, double T, double &) const;
              //! d(w)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_w_d_p_h_ph(double p, double h, double &) const;
              //! d(w)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_w_d_h_p_ph(double p, double h, double &) const;
              //! g(p,h)
              virtual EOS_Internal_Error compute_g_ph(double p, double h, double &) const;
              //! g(p,T)
              virtual EOS_Internal_Error compute_g_pT(double p, double T, double &) const;
              //! d(g)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_g_d_p_h_ph(double p, double h, double &) const;
              //! d(g)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_g_d_h_p_ph(double p, double h, double &) const;
              //! f(p,h)
              virtual EOS_Internal_Error compute_f_ph(double p, double h, double &) const;
              //! f(p,T)
              virtual EOS_Internal_Error compute_f_pT(double p, double T, double &) const;
              //! d(f)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_f_d_p_h_ph(double p, double h, double &) const;
              //! d(f)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_f_d_h_p_ph(double p, double h, double &) const;
              //! pr(p,h)
              virtual EOS_Internal_Error compute_pr_ph(double p, double h, double &) const;
              //! pr(p,T)
              virtual EOS_Internal_Error compute_pr_pT(double p, double T, double &) const;
              //! d(pr)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_pr_d_p_h_ph(double p, double h, double &) const;
              //! d(pr)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_pr_d_h_p_ph(double p, double h, double &) const;
              //! beta(p,h)
              virtual EOS_Internal_Error compute_beta_ph(double p, double h, double &) const;
              //! beta(p,T)
              virtual EOS_Internal_Error compute_beta_pT(double p, double T, double &) const;
              //! d(beta)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_beta_d_p_h_ph(double p, double h, double &) const;
              //! d(beta)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_beta_d_h_p_ph(double p, double h, double &) const;
              //! gamma(p,h)
              virtual EOS_Internal_Error compute_gamma_ph(double p, double h, double &) const;
              //! gamma(p,T)
              virtual EOS_Internal_Error compute_gamma_pT(double p, double T, double &) const;
              //! d(gamma)/dp      at constant specific enthalpy
              virtual EOS_Internal_Error compute_d_gamma_d_p_h_ph(double p, double h, double &) const;
              //! d(gamma)/dh      at constant pressure
              virtual EOS_Internal_Error compute_d_gamma_d_h_p_ph(double p, double h, double &) const;
              //! rho_l_sat
              virtual EOS_Internal_Error compute_rho_l_sat_p(double p, double &) const;
              virtual EOS_Internal_Error compute_d_rho_l_sat_d_p_p(double p, double &) const;
              //! rho_v_sat
              virtual EOS_Internal_Error compute_rho_v_sat_p(double p, double &) const;
              virtual EOS_Internal_Error compute_d_rho_v_sat_d_p_p(double p, double &) const;
              //! h_l_sat
              virtual EOS_Internal_Error compute_h_l_sat_p(double p, double &) const;
              virtual EOS_Internal_Error compute_d_h_l_sat_d_p_p(double p, double &) const;
              //! h_v_sat
              virtual EOS_Internal_Error compute_h_v_sat_p(double p, double &) const;
              virtual EOS_Internal_Error compute_d_h_v_sat_d_p_p(double p, double &) const;
              //! cp_l_sat
              virtual EOS_Internal_Error compute_cp_l_sat_p(double p, double &) const;
              virtual EOS_Internal_Error compute_d_cp_l_sat_d_p_p(double p, double &) const;
              //! cp_v_sat
              virtual EOS_Internal_Error compute_cp_v_sat_p(double p, double &) const;
              virtual EOS_Internal_Error compute_d_cp_v_sat_d_p_p(double p, double &) const;
              //! T_sat
              virtual EOS_Internal_Error compute_T_sat_p(double p, double &) const;
              virtual EOS_Internal_Error compute_d_T_sat_d_p_p(double p, double &) const;
              //! h_l_lim
              virtual EOS_Internal_Error compute_h_l_lim_p(double p, double &) const;
              //! h_v_lim
              virtual EOS_Internal_Error compute_h_v_lim_p(double p, double &) const;

       protected:
              EOS_Fields nodes;

              AString method;
              AString reference;

              AString base_method;
              AString base_reference;

              AString med_file;

              double pmin;
              double pmax;
              double hmin;
              double hmax;
              double tmin;
              double tmax;
              mutable double pmin_ipp;
              mutable double pmax_ipp;
              mutable double hmin_ipp;
              mutable double hmax_ipp;
              mutable double tmin_ipp;
              mutable double tmax_ipp;
              mutable double pmin_cpt;
              mutable double pmax_cpt;
              mutable double hmin_cpt;
              mutable double hmax_cpt;
              mutable double tmin_cpt;
              mutable double tmax_cpt;
              mutable int save_bound;
              Interpolation_Method interp_method; // BILINEAR by default (compatibility)
              double erreurtot; // interpolation error over the mesh
              double tcrit;
              double pcrit;
              double hcrit;
              double delta_p_f;
              double delta_h_f;
              unsigned int nb_p_virtual;
              unsigned int nb_h_virtual;

              // Load db med file
              EOS_Fields nodes_ph;
              EOS_Fields nodes_sat;
              EOS_Fields nodes_lim;
              EOS_Fields val_prop_properties;
              ArrOfInt connect_ph;
              ArrOfInt index_conn_ph;
              ArrOfInt connect_sat;
              ArrOfInt connect_lim;

              ArrOfDouble n_p_ph;
              ArrOfDouble n_h_ph;
              ArrOfDouble n_p_satlim;
              vector<ArrOfDouble> all_prop_val;
              vector<ArrOfInt> all_err_val;

              // pre-traitement
              // EOS_Fields fm_ph;
              vector<EOS_Error_Field*> err_cell_ph;
              vector<EOS_Error_Field*> err_segm_sat;
              vector<EOS_Error_Field*> err_segm_lim;

              // void make_f_mesh();
              void f_mesh2r_mesh();
              void f_mesh1r_mesh();
              void node_err2mesh_err(EOS_Property prop, EOS_Error_Field &val_nodes_ph);
              void node_err2segm_err(EOS_Property prop, EOS_Error_Field &val_nodes_p, int satlim);

              EOS_Internal_Error compute_prop_ph(EOS_Property prop,
                                                 double p, double h, double &res) const;
              EOS_Internal_Error compute_prop_p(EOS_Property prop,
                                                double p, int tag, double &res) const;

              // Retrieve the values of a cell for a given field as well as the associated ph values at the vertices.
              EOS_Internal_Error get_cell_values(int idx, EOS_Property i_prop, EOS_Ipp_CellData &cell_val) const;

              EOS_Internal_Error get_segm_values(int idx, EOS_Property i_prop, int tag, EOS_Ipp_CellData &segm_val) const;

              EOS_Internal_Error compute_h_l_pT(double p, double T, double &res) const;
              EOS_Internal_Error compute_h_v_pT(double p, double T, double &res) const;
              //! Saturation enthalpy on the liquid ('liquid' true) or vapor side,
              //! as used by compute_h_pT to check the phase of the root it inverted.
              //! Prefers the stored 1D saturation curve over re-inverting T(p,h) at
              //! T_sat on the 2D mesh -- cheaper, and the only route that works on a
              //! tiled database (cf. EOS_Ipp.cxx).
              EOS_Internal_Error compute_h_sat_for_phase(double p, double T_sat, bool liquid,
                                                         double &res) const;
              // Inversion h(p,T) on the bicubic (Hermite patch) representation of
              // T(p,h): used by compute_h_l_pT / compute_h_v_pT when the BICUBIC
              // method is selected and the T derivative fields are available.
              // Same cell-scanning strategy as the bilinear inversion (cf. report
              // Doc/Interpolator), but the per-cell equation T(p,h) = T is cubic in
              // h at fixed p and is solved in closed form (cf. EOS_Ipp.cxx).
              EOS_Internal_Error compute_h_pT_bicubic(double p, double T, double &res) const;
              EOS_Internal_Error check_p_bounds_ph(double p) const;

              virtual EOS_Error init_model(const std::string &model_name, const std::string &fluid_name, bool switch_comp_sat, bool swch_calc_deriv_fld); // for the interpolator
              virtual EOS_Error compute(const EOS_Field &p, const EOS_Field &h, EOS_Fields &r,
                                        EOS_Error_Field &errfield) const;
              EOS_Error compute(const EOS_Field &p, EOS_Fields &r, EOS_Error_Field &errfield) const;

              /* Function used to retrieve the bounds of a dataset (runs the calculations with the fluid declared via init_model) */
              virtual EOS_Error compute_(const EOS_Field &p, const EOS_Field &h, EOS_Fields &r,
                                         EOS_Error_Field &errfield) const;
              /* Function used to retrieve the bounds of a dataset (runs the calculations with the fluid declared via init_model) */
              EOS_Error compute_(const EOS_Field &p, EOS_Fields &r, EOS_Error_Field &errfield) const;

       private:
              static int type_Id;
              AString FluidStr;

              // Non-null only in "tiled database" (streaming) mode: init() detected a
              // ".eosmm" manifest instead of a plain .med file. When set, compute_prop_ph,
              // compute_prop_p and compute_h_pT delegate to it instead of running their
              // usual body against this instance's own (in that mode, unused) nodes_ph /
              // connect_ph / ... members. NULL in the historical, whole-database mode, so
              // every existing caller keeps the exact previous behaviour.
              EOS_Ipp_TileCache *tile_cache_ = nullptr;
              //! properties, when non-empty, restricts every tile load to those fields.
              int init_tiled(AString file_name, const Strings &properties);

              // True when this batch is a (p,T) request whose h(p,T) inversion is
              // worth doing once per point rather than once per point and per
              // output field, and sets p_field/T_field to the two inputs in a
              // known order. EOS_Fluid::compute loops fields on the outside and
              // re-derives h for each of them, which on this class means
              // re-scanning a whole p-column per field. Declines for fewer than
              // two outputs (nothing to share) and when h is itself an output
              // (cf. EOS_Ipp.cxx).
              bool hoistable_h_pT(const EOS_Field &pp, const EOS_Field &hh,
                                  const EOS_Fields &r,
                                  const EOS_Field *&p_field, const EOS_Field *&T_field) const;

              // Tiled mode: runs a (p,h) batch with its points regrouped by tile,
              // so a batch spanning more tiles than the cache can hold does not
              // reload them all the way through. Only the order changes -- the
              // permuted fields go through the same EOS_Fluid::compute dispatch and
              // the results are scattered back. Returns false (having done nothing)
              // when regrouping would not pay for itself.
              bool compute_tiled_regrouped(const EOS_Field &pp, const EOS_Field &hh,
                                           EOS_Fields &r, EOS_Error_Field &errfield,
                                           EOS_Error &result) const;

              ArrOfInt corners;        // list of the 4 nodes forming the corners of each cell of the
                                       // non-conforming mesh. Size: 4 * nb_cells_med_mesh
                                       // vertex i of cell j -> corners[i + 4*j]
              ArrOfInt fnodes2phnodes; // correspondence between each cell of the ph mesh and the cell
                                       // of the non-conforming (med) mesh it is in
              ArrOfInt fnodes2pnodes; // correspondence between each cell of the p mesh and the cell in the saturation regime
              ArrOfInt fnodes2pnodes_lim; // correspondence between each cell of the p mesh and the cell in the limit regime
                                       //
              // Strips the ":"-separated options a reference name may carry after the
              // file name and applies them, leaving file_name holding the bare name.
              // Recognized:
              //   bicubic | bilinear     interpolation method on the 2D (p,h) mesh
              //   cache=<n>[MB|GB]       tiled databases: resident tile budget, in bytes
              //   tiles=<n>              tiled databases: resident tile budget, in tiles
              // e.g. "water.eosmm:bicubic:cache=512MB". Historical names carrying just
              // ":bicubic"/":bilinear" keep working unchanged.
              void extract_init_options(AString &file_name);

              // Resident tile budget for tiled mode, as set by extract_init_options or
              // by the EOS_IPP_TILE_CACHE env variable. Zero means "unset": the
              // EOS_Ipp_TileCache default applies.
              std::size_t tile_cache_bytes_ = 0;
              std::size_t tile_cache_tiles_ = 0;

              void load_domain_values(EOS_Med &med);
              EOS_Error load_med_nodes(EOS_Med &med);
              EOS_Error load_med_champ(EOS_Med &med);
              EOS_Error load_med_champ(EOS_Med &med, const Strings &properties);
              EOS_Error load_med_scalar(EOS_Med &med);

              int get_cellidx(double &p, double &h) const;
              // Real (med) cells whose p-range contains p, in ascending h order:
              // the cell list scanned by the h(p,T) inversions. Each identified
              // cell is used to jump directly over its own h-extent, so the cost
              // is O(number of real cells in the p-column), not O(nb_h_virtual).
              // The cell list is appended to 'cells' (cleared first) rather than
              // returned by value: the h(p,T) inversions call this per point, and
              // a fresh vector per point is an allocation the caller can hoist.
              void get_cells_containing_p(double p, std::vector<unsigned int> &cells) const;
              int get_segmidx(double &p, int sat_lim) const;
              double linear_interpolator(double p, const EOS_Ipp_CellData &segmval) const;
              //void bilinear_interpolator(double p, double h, double &res) const;
              double bilinear_interpolator(double p, double h, const EOS_Ipp_CellData &cellval) const;

              // Bicubic (Hermite patch) interpolation on the 2D (p,h) mesh.
              // cellval rows: [0]=p, [1]=h, [2]=f, [3]=d f/dp |h, [4]=d f/dh |p,
              // [5]=d2 f/dp.dh (4 corners each). Row [5] is only read when
              // has_cross_derivative is true; otherwise the cross derivative is
              // approximated locally from rows [3]/[4] (cf. EOS_Ipp.cxx).
              double bicubic_interpolator(double p, double h, const EOS_Ipp_CellData &cellval,
                                           bool has_cross_derivative) const;
              // Extracts the Hermite patch data of a cell in unit-square coordinates
              // (t along p, u along h): corner values f and derivatives ft = df/dt,
              // fu = df/du, ftu = d2f/dt.du, scaled from the physical derivatives of
              // cellval. Corner order: 0=(t=0,u=0), 1=(0,1), 2=(1,1), 3=(1,0). When
              // has_cross_derivative is false, ftu is the local twist approximation
              // (cf. EOS_Ipp.cxx). Shared by bicubic_interpolator (direct evaluation)
              // and compute_h_pT_bicubic (inversion), so both use the same patch.
              void bicubic_patch_data(const EOS_Ipp_CellData &cellval, bool has_cross_derivative,
                                       double f[4], double ft[4], double fu[4], double ftu[4]) const;
              // Fetches f and its two first partial derivatives at the 4 corners of the
              // cell (rows 0-4), plus the stored cross derivative (row 5) if
              // fetch_cross_derivative is true.
              EOS_Internal_Error get_cell_values_bicubic(int idx, EOS_Property i_prop, EOS_Ipp_CellData &cell_val,
                                                          bool fetch_cross_derivative) const;

              EOS_Internal_Error check_ph_bounds(double p, double h) const;
              EOS_Internal_Error check_p_bounds_satlim(double p) const;
       };
}

#include "EOS_Ipp_i.hxx"
#endif /* EOS_IPP_HXX_ */
