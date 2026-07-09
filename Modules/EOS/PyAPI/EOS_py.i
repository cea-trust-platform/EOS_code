%module eos_py

%{
#include "EOS_py.hxx"
%}

%include "std_string.i"
%include "std_vector.i"
%include "exception.i"

namespace std {
    %template(DoubleVector) vector<double>;
    %template(StringVector) vector<string>;
    %template(DoubleVectorVector) vector<vector<double>>;
}

// Without this, a C++ exception thrown anywhere below (EOS_py, EOS_IGen_py,
// EOS_Mixing_py all use std::runtime_error for error reporting, e.g. bad
// input, out-of-domain point, uninitialized object) is NOT translated into a
// catchable Python exception: it calls std::terminate and kills the whole
// Python process/kernel instead. This wraps every wrapped call in a
// try/catch and re-raises it as a Python RuntimeError.
%exception {
    try {
        $action
    } catch (const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    } catch (...) {
        SWIG_exception(SWIG_UnknownError, "unknown C++ exception in eos_py");
    }
}

%include "EOS_py.hxx"

// ----------------------------------------------------------------------------
// Pure-Python convenience layer, appended verbatim to the generated eos_py.py
// so it is available directly via "import eos_py" (no separate module/path
// needed). Built on top of EOS_IGen_py above: generate_mesh_igen() chains
// set_extremum -> make_mesh -> [set_list_properties] -> write_med with a
// signature closer to a generic mesh-generation tool, and documents where
// the real EOS_IGen API is more restrictive than that signature suggests
// (cf. its docstring). See EOS/api_python_interpolateur/
// documentation_api_python_interpolateur.ipynb for a full worked example.
// ----------------------------------------------------------------------------
%pythoncode %{
from dataclasses import dataclass, field
from typing import List, Optional, Sequence


@dataclass
class IGenMesh:
    """Result of a `generate_mesh_igen` call: describes the .med base
    written by EOS_IGen and gives direct access to a ready-to-use EOS_Ipp
    interpolator.

    Attributes
    ----------
    file_name : str
        Name (without .med extension) of the base written under
        ``$USER_EOS_DATA/EOS_Ipp/<file_name>.med``, registered in
        ``index.eos`` under this same name.
    method, reference : str
        Fluid model (e.g. "EOS_Cathare2") and reference (e.g. "WaterLiquid")
        used by EOS_IGen to compute values at each node.
    p_min, p_max, T_min, T_max : float
        Domain bounds (Pa, K) as passed to `generate_mesh_igen`.
    h_min, h_max : float
        Enthalpy bounds (J/kg) of the actually-built (p,h) mesh -- derived
        by EOS_IGen from the domain's 4 (p,T) corners, so only known AFTER
        construction (read back from the opened interpolator, not
        recomputed here).
    nx, ny : int
        Number of nodes along p and T/h (same values as nb_mesh_p,
        nb_mesh_h passed to EOS_IGen::make_mesh).
    level_max : int
        Refinement level passed to EOS_IGen::make_mesh (-1 = none, regular
        mesh -- cf. limits in generate_mesh_igen's docstring).
    properties : list[str] or None
        Explicitly requested property list (None if EOS_IGen's default list
        -- every entry of Modules/EOS/API/therm_properties.hxx -- was used).
    """
    file_name: str
    method: str
    reference: str
    p_min: float
    p_max: float
    T_min: float
    T_max: float
    h_min: float
    h_max: float
    nx: int
    ny: int
    level_max: int
    properties: Optional[List[str]] = field(default=None)

    def open_interpolator(self):
        """Opens and returns a new ``EOS_py("EOS_Ipp", ...)`` object
        pointing at this base -- equivalent to calling
        ``eos_py.EOS_py("EOS_Ipp", mesh.file_name)`` directly, provided here
        for convenience so generation and interpolation can be chained
        without repeating the file name.

        A new object is created on every call (the EOS_Ipp object is not
        cached in IGenMesh): calling this repeatedly reloads the base each
        time, which has a cost (cf. the performance report -- a few
        milliseconds per call in the cases studied, not free if repeated
        thousands of times).
        """
        return EOS_py("EOS_Ipp", self.file_name)


def generate_mesh_igen(
    method,
    reference,
    x_min,
    x_max,
    y_min,
    y_max,
    nx,
    ny,
    x_variable="P",
    y_variable="T",
    scale_x="linear",
    scale_y="linear",
    level_max=-1,
    properties=None,
    file_name=None,
):
    """Generates an EOS interpolation mesh with EOS_IGen and writes the
    corresponding .med base, ready to be loaded by
    ``eos_py.EOS_py("EOS_Ipp", file_name)``.

    Reuses the EOS_IGen_py class (SWIG wrapper of
    NEPTUNE_EOS_IGEN::EOS_IGen, cf. Modules/EOS/PyAPI/EOS_py.hxx/.cxx),
    which chains set_extremum -> make_mesh -> [set_list_properties] ->
    write_med. This function only assembles those four calls and validates
    parameters before passing them on -- all the generation logic stays in
    EOS_IGen (C++), nothing is recomputed here.

    Parameters
    ----------
    method : str
        EOS fluid model name, as used everywhere else in the project (e.g.
        "EOS_Cathare2", "EOS_Refprop9") -- this is the first argument
        expected by ``EOS_py(method, reference)``.
    reference : str
        Fluid reference for this model (e.g. "WaterLiquid").
    x_min, x_max : float
        Domain bounds along the abscissa, in Pa (x_variable must be "P",
        cf. Limitations below).
    y_min, y_max : float
        Domain bounds along the ordinate, in K (y_variable must be "T",
        cf. Limitations below).
    nx, ny : int
        Number of mesh nodes along p and T/h (each >= 2 -- constraint of
        EOS_IGen::make_mesh). The mesh actually stored is a (p,h) mesh: the
        h bounds are derived internally by EOS_IGen from the domain's 4
        (p,T) corners (cf. EOS_IGen::make_mesh,
        Modules/EOS_IGen/API/EOS_IGen.cxx), not chosen directly.
    x_variable : str, optional
        Name of the abscissa variable. **Must be "P"**: raises
        NotImplementedError otherwise (cf. Limitations). Present to make
        the call's intent explicit, and for possible future use if
        EOS_IGen comes to support other variable pairs.
    y_variable : str, optional
        Name of the ordinate variable. **Must be "T"**: raises
        NotImplementedError otherwise (cf. Limitations).
    scale_x, scale_y : str, optional
        Mesh scale type along each axis. **Must be "linear"**: raises
        NotImplementedError for any other value (e.g. "log"), cf.
        Limitations.
    level_max : int, optional (default -1)
        Refinement level passed to EOS_IGen::make_mesh. -1 (default) means
        a regular mesh with no refinement -- the only value tested by this
        project (cf. Limitations: quality-criterion-driven adaptive
        refinement is not exposed by this wrapper).
    properties : sequence of str, optional
        List of properties to compute/store at each node (EOS_IGen
        canonical names, e.g. "T", "d_T_d_p_h", "d_T_d_h_p",
        "d2_T_d_p_d_h" -- cf. Modules/EOS/API/therm_properties.hxx). If
        None (default), EOS_IGen uses its full default property list,
        which is slower to generate.
    file_name : str, optional
        Name (without .med extension) of the base to write under
        ``$USER_EOS_DATA/EOS_Ipp/<file_name>.med``. If None (default), a
        name is generated automatically from method/reference/nx/ny, e.g.
        "igen_EOS_Cathare2_WaterLiquid_50x50".

    Returns
    -------
    IGenMesh
        A small object describing the written base (file name, p/T/h
        domain, size, properties) and offering ``.open_interpolator()`` to
        directly obtain a ready-to-interpolate ``EOS_py("EOS_Ipp", ...)``.

    Example
    -------
    >>> import eos_py
    >>> mesh = eos_py.generate_mesh_igen(
    ...     method="EOS_Cathare2", reference="WaterLiquid",
    ...     x_min=1.0e7, x_max=2.0e7, y_min=300.0, y_max=500.0,
    ...     nx=20, ny=20,
    ...     properties=["T", "d_T_d_p_h", "d_T_d_h_p"],
    ...     file_name="demo_mesh_20x20",
    ... )
    >>> mesh.file_name
    'demo_mesh_20x20'
    >>> ipp = mesh.open_interpolator()
    >>> ipp.compute("p", "h", [1.5e7], [5.0e5], ["T"])
    [[...]]

    Limitations
    -----------
    - **x_variable/y_variable fixed to "P"/"T"**: EOS_IGen::set_extremum
      (cf. Modules/EOS_IGen/API/EOS_IGen.hxx) only takes (p_min, p_max,
      T_min, T_max) -- there is, in the current code, no way to define a
      mesh over another pair of variables (e.g. (rho, T)). These two
      parameters are nonetheless accepted here, validated then rejected
      with an explicit message if not "P"/"T", rather than silently
      ignored -- to stay close to a generic mesh-generator signature while
      never claiming a capability absent from the real code.
    - **scale_x/scale_y fixed to "linear"**: no logarithmic-scale option
      exists in EOS_IGen or EOS_Mesh (Modules/EOS_IGen/Src/EOS_Mesh.*) --
      the mesh is always regular (constant step) in p and T. Same logic as
      above: value explicitly rejected rather than ignored.
    - **Adaptive refinement not exposed**: EOS_IGen separately offers
      quality-criterion-driven refinement (set_quality, compute_qualities,
      make_local_refine, make_global_refine, class EOS_IGen_QI): this
      function does not expose it (only make_mesh's level_max is, value -1
      only tested). Use the C++ API directly, or extend EOS_IGen_py, for
      that need.
    - **(p,T) domain only, not (p,h) directly**: even though the mesh ends
      up stored in (p,h), bounds must be given in (p,T) --
      EOS_IGen::make_mesh_ph exists on the C++ side for a direct (p,h) mesh
      but is not exposed by EOS_IGen_py in this first version.
    - **No a priori physical-domain check**: if a (p,T) domain corner falls
      outside the chosen fluid model's valid range, the error only occurs
      when make_mesh is called (Python exception raised), not before.
    """
    if x_variable != "P":
        raise NotImplementedError(
            "generate_mesh_igen: x_variable='{}' not supported -- EOS_IGen only "
            "builds a mesh in the (P, T) plane; x_variable must be 'P' "
            "(cf. the docstring's Limitations section).".format(x_variable)
        )
    if y_variable != "T":
        raise NotImplementedError(
            "generate_mesh_igen: y_variable='{}' not supported -- EOS_IGen only "
            "builds a mesh in the (P, T) plane; y_variable must be 'T' "
            "(cf. the docstring's Limitations section).".format(y_variable)
        )
    if scale_x != "linear" or scale_y != "linear":
        raise NotImplementedError(
            "generate_mesh_igen: scale_x/scale_y='{}'/'{}'  not supported -- "
            "no logarithmic-scale option exists in EOS_IGen: the mesh is "
            "always regular (constant step) in P and T "
            "(cf. the docstring's Limitations section).".format(scale_x, scale_y)
        )

    if file_name is None:
        file_name = "igen_{}_{}_{}x{}".format(method, reference, nx, ny)

    igen = EOS_IGen_py(method, reference)
    igen.set_extremum(x_min, x_max, y_min, y_max)

    prop_list = list(properties) if properties is not None else None
    if prop_list is not None:
        igen.set_list_properties(prop_list)

    igen.make_mesh(nx, ny, level_max)
    igen.write_med(file_name)

    # h bounds are only known after construction (derived by EOS_IGen from
    # the domain's (p,T) corners): read back from the freshly-written base
    # rather than re-deriving them ourselves.
    ipp = EOS_py("EOS_Ipp", file_name)

    return IGenMesh(
        file_name=file_name,
        method=method,
        reference=reference,
        p_min=x_min, p_max=x_max,
        T_min=y_min, T_max=y_max,
        h_min=ipp.get_h_min(), h_max=ipp.get_h_max(),
        nx=nx, ny=ny,
        level_max=level_max,
        properties=prop_list,
    )
%}