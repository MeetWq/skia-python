#include "common.h"
#include <pybind11/stl.h>

void initColorSpaceDeclarations(py::module &m) {
// TODO: Implement skcms APIs.
py::module skcms = m.def_submodule("cms");

// TODO: Implement trc, toXYZD50, and A2B.
py::class_<skcms_ICCProfile>(skcms, "ICCProfile");

py::class_<skcms_TransferFunction>(skcms, "TransferFunction",
    R"docstring(
    A transfer function mapping encoded values to linear values,
    represented by this 7-parameter piecewise function:

      linear = sign(encoded) *  (c*|encoded| + f)       , 0 <= |encoded| < d
             = sign(encoded) * ((a*|encoded| + b)^g + e), d <= |encoded|

    (A simple gamma transfer function sets g to gamma and a to 1.)
    )docstring");

py::class_<skcms_Matrix3x3>(skcms, "Matrix3x3",
    R"docstring(
    A row-major 3x3 matrix (ie vals[row][col])
    )docstring");

py::class_<SkColorSpace, sk_sp<SkColorSpace>>(m, "ColorSpace");
}

void initColorSpaceDefinitions(py::module &m) {
auto skcms = static_cast<py::module>(m.attr("cms"));

auto iccprofile = static_cast<py::class_<skcms_ICCProfile>>(skcms.attr("ICCProfile"));
iccprofile
    .def(py::init<>())
    .def_readwrite("buffer", &skcms_ICCProfile::buffer)
    .def_readwrite("size", &skcms_ICCProfile::size)
    .def_readwrite("data_color_space", &skcms_ICCProfile::data_color_space)
    .def_readwrite("pcs", &skcms_ICCProfile::pcs)
    .def_readwrite("tag_count", &skcms_ICCProfile::tag_count)
    .def_readwrite("has_trc", &skcms_ICCProfile::has_trc)
    .def_readwrite("has_toXYZD50", &skcms_ICCProfile::has_toXYZD50)
    .def_readwrite("has_A2B", &skcms_ICCProfile::has_A2B)
    ;

auto transferfunction = static_cast<py::class_<skcms_TransferFunction>>(
    skcms.attr("TransferFunction"));
transferfunction
    .def(py::init(
        [] (const std::vector<float>& v) {
            if (v.size() != 7)
                throw py::value_error("Number of elements must be 9.");
            return skcms_TransferFunction {
                v[0], v[1], v[2], v[3], v[4], v[5], v[6]
            };
        }), py::arg("v"));

auto matrix3x3 = static_cast<py::class_<skcms_Matrix3x3>>(skcms.attr("Matrix3x3"));
matrix3x3
    .def(py::init(
        [] (const std::vector<float>& v) {
            if (v.size() != 9)
                throw py::value_error("Number of elements must be 9.");
            return skcms_Matrix3x3 {{
                {v[0], v[1], v[2]},
                {v[3], v[4], v[5]},
                {v[6], v[7], v[8]}
            }};
        }), py::arg("v"));

auto colorspace = static_cast<py::class_<SkColorSpace, sk_sp<SkColorSpace>>>(
    m.attr("ColorSpace"));
colorspace
    .def("toProfile",
        [] (const SkColorSpace& colorspace) {
            std::unique_ptr<skcms_ICCProfile> profile(new skcms_ICCProfile());
            colorspace.toProfile(profile.get());
            return profile;
        },
        R"docstring(
        Convert this color space to an skcms ICC profile struct.
        )docstring")
    .def("gammaCloseToSRGB", &SkColorSpace::gammaCloseToSRGB,
        R"docstring(
        Returns true if the color space gamma is near enough to be approximated
        as sRGB.
        )docstring")
    .def("gammaIsLinear", &SkColorSpace::gammaIsLinear,
        R"docstring(
        Returns true if the color space gamma is linear.
        )docstring")
    .def("isNumericalTransferFn", &SkColorSpace::isNumericalTransferFn,
        R"docstring(
        Sets ``|fn|`` to the transfer function from this color space.

        Returns true if the transfer function can be represented as coefficients
        to the standard ICC 7-parameter equation. Returns false otherwise (eg,
        PQ, HLG).
        )docstring",
        py::arg("fn"))
    .def("toXYZD50", &SkColorSpace::toXYZD50,
        R"docstring(
        Returns true and sets ``|toXYZD50|`` if the color gamut can be described
        as a matrix.

        Returns false otherwise.
        )docstring",
        py::arg("matrix"))
    .def("toXYZD50Hash", &SkColorSpace::toXYZD50Hash,
        R"docstring(
        Returns a hash of the gamut transformation to XYZ D50.

        Allows for fast equality checking of gamuts, at the (very small) risk of
        collision.
        )docstring")
    .def("makeLinearGamma", &SkColorSpace::makeLinearGamma,
        R"docstring(
        Returns a color space with the same gamut as this one, but with a linear
        gamma.

        For color spaces whose gamut can not be described in terms of XYZ D50,
        returns linear sRGB.
        )docstring")
    .def("makeSRGBGamma", &SkColorSpace::makeSRGBGamma,
        R"docstring(
        Returns a color space with the same gamut as this one, with with the
        sRGB transfer function.

        For color spaces whose gamut can not be described in terms of XYZ D50,
        returns sRGB.
        )docstring")
    .def("makeColorSpin", &SkColorSpace::makeColorSpin,
        R"docstring(
        Returns a color space with the same transfer function as this one, but
        with the primary colors rotated.

        For any XYZ space, this produces a new color space that maps RGB to GBR
        (when applied to a source), and maps RGB to BRG (when applied to a
        destination). For other types of color spaces, returns nullptr.

        This is used for testing, to construct color spaces that have severe and
        testable behavior.
        )docstring")
    .def("isSRGB", &SkColorSpace::isSRGB,
        R"docstring(
        Returns true if the color space is sRGB.

        Returns false otherwise.

        This allows a little bit of tolerance, given that we might see small
        numerical error in some cases: converting ICC fixed point to float,
        converting white point to D50, rounding decisions on transfer function
        and matrix.

        This does not consider a 2.2f exponential transfer function to be sRGB.
        While these functions are similar (and it is sometimes useful to
        consider them together), this function checks for logical equality.
        )docstring")
    .def("serialize", &SkColorSpace::serialize,
        R"docstring(
        Returns nullptr on failure.

        Fails when we fallback to serializing ICC data and the data is too large
        to serialize.
        )docstring")
    .def("writeToMemory", &SkColorSpace::writeToMemory,
        R"docstring(
        If ``|memory|`` is nullptr, returns the size required to serialize.

        Otherwise, serializes into ``|memory|`` and returns the size.
        )docstring",
        py::arg("memory"))
    .def("transferFn",
        py::overload_cast<float[7]>(&SkColorSpace::transferFn, py::const_),
        py::arg("gabcdef"))
    .def("transferFn",
        py::overload_cast<skcms_TransferFunction*>(
            &SkColorSpace::transferFn, py::const_),
        py::arg("fn"))
    .def("invTransferFn", &SkColorSpace::invTransferFn,
        py::arg("fn"))
    .def("gamutTransformTo", &SkColorSpace::gamutTransformTo,
        py::arg("dst"), py::arg("src_to_dst"))
    .def("transferFnHash", &SkColorSpace::transferFnHash)
    .def("hash", &SkColorSpace::hash)
    .def("unique", &SkColorSpace::unique)
    .def("ref", &SkColorSpace::ref)
    .def("unref", &SkColorSpace::unref)
    .def("deref", &SkColorSpace::deref)
    .def("refCntGreaterThan", &SkColorSpace::refCntGreaterThan,
        py::arg("count"))
    .def_static("MakeSRGB", &SkColorSpace::MakeSRGB,
        R"docstring(
        Create the sRGB color space.
        )docstring")
    .def_static("MakeSRGBLinear", &SkColorSpace::MakeSRGBLinear,
        R"docstring(
        Colorspace with the sRGB primaries, but a linear (1.0) gamma.
        )docstring")
    .def_static("MakeRGB", &SkColorSpace::MakeRGB,
        R"docstring(
        Create an :py:class:`ColorSpace` from a transfer function and a
        row-major 3x3 transformation to XYZ.
        )docstring",
        py::arg("transferFn"), py::arg("toXYZ"))
    .def_static("Make", &SkColorSpace::Make,
        R"docstring(
        Create an :py:class:`ColorSpace` from a parsed (skcms) ICC profile.
        )docstring",
        py::arg("profile"))
    .def_static("Deserialize", &SkColorSpace::Deserialize,
        py::arg("data"), py::arg("length"))
    .def_static("Equals", &SkColorSpace::Equals,
        R"docstring(
        If both are null, we return true.

        If one is null and the other is not, we return false. If both are
        non-null, we do a deeper compare.
        )docstring",
        py::arg("self"), py::arg("other"))
    .def("__eq__", &SkColorSpace::Equals, py::is_operator())
    ;
}
