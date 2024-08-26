#include "common.h"
#include <include/core/SkScalar.h>

void initScalarDeclarations(py::module &m) {
}

void initScalarDefinitions(py::module &m) {
    m.attr("ScalarInfinity") = py::cast(std::numeric_limits<float>::infinity());
    m.attr("ScalarNegativeInfinity") = py::cast(-std::numeric_limits<float>::infinity());
}
