#include "common.h"
#include <include/core/SkSamplingOptions.h>

/*
  Reference:

  https://github.com/google/skia/commit/95cc53bafd5fcd94b55a22450457cfde1dd0fd37
  "Hide filterquality enum entirely"

  The last of SkFilterQuality emulation is:

  FilterQuality.kHigh_FilterQuality   -> SamplingOptions(CubicResampler.Mitchell())
  FilterQuality.kMedium_FilterQuality -> SamplingOptions(FilterMode.kLinear, MipmapMode.kNearest)    // cpu
                                           or SamplingOptions(FilterMode.kLinear, MipmapMode.kLinear)  // gpu
  FilterQuality.kLow_FilterQuality    -> SamplingOptions(FilterMode.kLinear, MipmapMode.kNone)
  FilterQuality.kNone_FilterQuality   -> SamplingOptions(FilterMode.kNearest, MipmapMode.kNone)
*/

void initSamplingOptionsDeclarations(py::module& m) {
py::class_<SkSamplingOptions> samplingoptions(m, "SamplingOptions");

py::enum_<SkFilterMode>(m, "FilterMode")
    .value("kNearest", SkFilterMode::kNearest,
        "single sample point (nearest neighbor)")
    .value("kLinear", SkFilterMode::kLinear,
        "interporate between 2x2 sample points (bilinear interpolation)")
    .export_values();

py::enum_<SkMipmapMode>(m, "MipmapMode")
    .value("kNone", SkMipmapMode::kNone,
        "ignore mipmap levels, sample from the \"base\"")
    .value("kNearest", SkMipmapMode::kNearest,
        "sample from the nearest level")
    .value("kLinear", SkMipmapMode::kLinear,
        "interpolate between the two nearest levels")
    .export_values();

py::class_<SkCubicResampler>(m, "CubicResampler")
    .def_readwrite("B", &SkCubicResampler::B)
    .def_readwrite("C", &SkCubicResampler::C)
    .def_static("Mitchell", &SkCubicResampler::Mitchell)
    .def_static("CatmullRom", &SkCubicResampler::CatmullRom);
}

void initSamplingOptionsDefinitions(py::module& m) {
auto samplingoptions = static_cast<py::class_<SkSamplingOptions>>(
    m.attr("SamplingOptions"));
samplingoptions
    .def(py::init<>())
    .def(py::init<SkSamplingOptions>(),
        py::arg("SamplingOptions"))
    .def(py::init<SkCubicResampler>(),
        py::arg("CubicResampler"))
    .def(py::init<SkFilterMode>(),
        py::arg("FilterMode"))
    .def(py::init<SkFilterMode, SkMipmapMode>(),
        py::arg("FilterMode"), py::arg("MipmapMode"))
    .def("isAniso", &SkSamplingOptions::isAniso)
    .def_readonly("maxAniso", &SkSamplingOptions::maxAniso)
    .def_readonly("useCubic", &SkSamplingOptions::useCubic)
    .def_readonly("cubic", &SkSamplingOptions::cubic)
    .def_readonly("filter", &SkSamplingOptions::filter)
    .def_readonly("mipmap", &SkSamplingOptions::mipmap)
    .def_static("Aniso", &SkSamplingOptions::Aniso, py::arg("maxAniso"))
    .def("__eq__",
        [](const SkSamplingOptions& self, const SkSamplingOptions& other) {
            return self == other;
        },
        py::is_operator())
    .def("__ne__",
        [](const SkSamplingOptions& self, const SkSamplingOptions& other) {
            return self != other;
        },
        py::is_operator());
}
