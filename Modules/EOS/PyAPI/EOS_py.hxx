#ifndef EOS_PY_HXX
#define EOS_PY_HXX

#include <vector>
#include <string>
#include "EOS/API/EOS.hxx"
#include <sstream>

// Forward declaration only: keeps the (heavier) EOS_IGen headers out of this
// file, included only in EOS_py.cxx -- same pattern as EOS_Ipp below.
namespace NEPTUNE_EOS_IGEN { class EOS_IGen; }

class EOS_py {
public:
    EOS_py(const std::string& meth);
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
    std::string describe() const;
    static void fluids_available(); 

    double get_p_crit() const;
    double get_h_crit() const;
    double get_T_crit() const;
    double get_rho_crit() const;

    double get_p_min() const;
    double get_p_max() const;

    double get_h_max() const;
    double get_h_min() const;

    double get_T_max() const;
    double get_T_min() const;

    double get_rho_max() const;
    double get_rho_min() const;

    double get_p() const;
    double get_mm() const;

    // Selects the interpolation method used on the 2D (p,h) mesh, when this
    // EOS_py object wraps an EOS_Ipp interpolator (method == "EOS_Ipp").
    // mode must be "bilinear" or "bicubic". Throws if this object does not
    // wrap an EOS_Ipp interpolator (cf. EOS_Ipp::Interpolation_Method).
    void set_interpolation_method(const std::string& mode);
    // Returns "bilinear" or "bicubic". Throws if this object does not wrap
    // an EOS_Ipp interpolator.
    std::string get_interpolation_method() const;

    // True if the loaded base has the two first-derivative fields for
    // prop_name (e.g. "T") -- the minimum required for BICUBIC. Throws if
    // this object does not wrap an EOS_Ipp interpolator.
    bool has_bicubic_first_derivative_data(const std::string& prop_name) const;
    // True if, in addition, the stored cross derivative (d2_<prop>_d_p_d_h)
    // is loaded -- used by the Hermite patch instead of the local twist
    // approximation when available (cf. EOS_Ipp::bicubic_interpolator).
    bool has_bicubic_cross_derivative_data(const std::string& prop_name) const;

private:
    NEPTUNE::EOS* eos_;

    // Helper générique pour éviter duplication
    double call_getter(
        NEPTUNE::EOS_Error (NEPTUNE::EOS::*func)(double&) const,
        const std::string& name
    ) const;
};


// Thin wrapper around NEPTUNE_EOS_IGEN::EOS_IGen : builds a regular (p,h)
// interpolation mesh over a rectangular (p,T) domain and writes it to a
// .med base readable by EOS_py("EOS_Ipp", file_name) (cf. EOS_py above).
//
// This exposes the same workflow already used by the project's own C++
// mesh-generation tools (e.g. Modules/EOS_IGen/Tests/C++/main.cxx,
// EOSIGenTest): set_extremum -> make_mesh -> [set_list_properties] ->
// write_med for a regular mesh, or set_extremum -> make_mesh(level_max) ->
// [set_list_properties] -> set_quality(...) -> make_global_refine() /
// make_local_refine() -> write_med for a quality-driven adaptively refined
// one. compute_qualities() itself is not exposed separately: it is called
// internally by make_global_refine/make_local_refine, exactly as in the
// C++ API.
//
// EOS_IGen only ever builds a mesh in the (p, T) plane (cf.
// EOS_IGen::set_extremum(p_min, p_max, T_min, T_max) and ::make_mesh in
// Modules/EOS_IGen/API/EOS_IGen.cxx): there is no way to choose another pair
// of abscissa/ordinate variables, and node spacing is always regular
// (linear) in p and T -- no logarithmic-scale option exists in the
// underlying C++ API. This wrapper does not invent either capability.
class EOS_IGen_py {
public:
    // method/reference identify the real fluid model used to compute
    // property values at each mesh node (e.g. "EOS_Cathare2"/"WaterLiquid"),
    // exactly as for EOS_py(method, reference).
    EOS_IGen_py(const std::string& method, const std::string& reference);
    ~EOS_IGen_py();

    // Sets the rectangular (p, T) domain bounds, in Pa and K. Must be called
    // before make_mesh(). Throws if p_min <= 0, p_max <= p_min or T_max <= T_min.
    void set_extremum(double p_min, double p_max, double T_min, double T_max);

    // Restricts the properties computed/stored at each mesh node (e.g.
    // {"T", "d_T_d_p_h", "d_T_d_h_p", "d2_T_d_p_d_h"}). Optional: if never
    // called, EOS_IGen falls back to its full default property list (every
    // entry of Modules/EOS/API/therm_properties.hxx). Must be called before
    // make_mesh() to take effect.
    void set_list_properties(std::vector<std::string> properties);

    // Builds a regular nb_mesh_p x nb_mesh_h (p,h) mesh spanning the domain
    // set by set_extremum (the h bounds are derived internally from the
    // real fluid model at the 4 (p,T) corners) and computes the requested
    // properties at each node. level_max=-1 (default) means no refinement.
    // Throws if set_extremum() was not called first, if nb_mesh_p/nb_mesh_h
    // < 2, or if the underlying EOS_IGen::make_mesh call fails (e.g. a
    // corner falls outside the fluid model's valid domain).
    void make_mesh(int nb_mesh_p, int nb_mesh_h, int level_max = -1);

    // Defines a mesh-refinement quality criterion, consumed by
    // make_global_refine()/make_local_refine(): at each candidate node (or
    // cell center, depending on type), the criterion compares the current
    // EOS_Ipp interpolated value of `property` to the real fluid model's
    // value, and flags the mesh as "not good enough" wherever the
    // difference exceeds limit_qi. May be called several times to combine
    // several criteria (all must pass). Must be called after make_mesh().
    //   property : e.g. "T", "rho", "mu" -- any property in the base.
    //   type     : "centre" (evaluate at cell centers) or "node" (at nodes).
    //   is_abs   : 1 for an absolute threshold on |Ipp - real|, 0 for a
    //              relative threshold on |Ipp - real| / |real|.
    //   limit_qi : the threshold itself.
    void set_quality(const std::string& property, const std::string& type,
                      int is_abs, double limit_qi);

    // Iteratively adds nodes UNIFORMLY over the whole mesh (uniform
    // halving of every cell) until every quality criterion set via
    // set_quality() is satisfied everywhere, or the level_max passed to
    // make_mesh() is reached. Must be called after make_mesh() and at
    // least one set_quality() call. Throws if the underlying call fails
    // (e.g. no quality criterion set, or a quality property not available
    // in the base).
    void make_global_refine();

    // Iteratively adds nodes only WHERE the quality criteria fail (as
    // opposed to make_global_refine's uniform refinement), until satisfied
    // or level_max is reached -- cheaper than global refinement for a
    // criterion that only fails in a small region (e.g. near a steep
    // gradient). cont=true (default) also adds "continuity nodes" at the
    // boundary between refined and unrefined regions, which EOS_Ipp
    // expects for a conforming mesh; cont=false produces a non-conforming
    // mesh, used by this project's own tests specifically to exercise
    // EOS_Ipp's handling of that case -- prefer cont=true unless you have
    // a specific reason not to. Must be called after make_mesh() and at
    // least one set_quality() call.
    void make_local_refine(bool cont = true);

    // Writes the final .med interpolation base (plus its index.eos entry)
    // under $USER_EOS_DATA/EOS_Ipp/<file_name>.med. Throws if make_mesh()
    // was not called first, or if the write fails (e.g. USER_EOS_DATA not
    // set/not writable).
    void write_med(const std::string& file_name);

    // Human-readable summary (method, reference, domain, mesh status) --
    // handy for notebooks/debugging, not a substitute for the getters below.
    std::string describe() const;

    double get_p_min() const;
    double get_p_max() const;
    double get_T_min() const;
    double get_T_max() const;

private:
    NEPTUNE_EOS_IGEN::EOS_IGen* igen_;
    std::string method_;
    std::string reference_;
    double p_min_;
    double p_max_;
    double T_min_;
    double T_max_;
    bool extremum_set_;
    bool mesh_built_;
};


class EOS_Mixing_py {
public:
    EOS_Mixing_py(
        std::vector<std::string> methods,
        std::vector<std::string> refs
    );

    ~EOS_Mixing_py();

    std::vector<std::vector<double>> compute(
        std::vector<std::string> input_names,
        std::vector<std::vector<double>> input_values,
        std::vector<std::string> output_names
    );
    std::string describe() const;

private:
    NEPTUNE::EOS* mixing_;
    std::vector<NEPTUNE::EOS*> components_;
};



#endif
