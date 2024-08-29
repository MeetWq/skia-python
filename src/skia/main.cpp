#include "common.h"
#include "include/core/SkGraphics.h"
#include "modules/svg/include/SkSVGOpenTypeSVGDecoder.h"

#define XSTRING(s) STRING(s)
#define STRING(s) #s

// Declarations.
void initBitmapDeclarations(py::module &);
void initBlendModeDeclarations(py::module &);
void initCanvasDeclarations(py::module &);
void initColorDeclarations(py::module &);
void initCodecDeclarations(py::module &);
void initColorSpaceDeclarations(py::module &);
void initDataDeclarations(py::module &);
void initDocumentDeclarations(py::module &);
void initGrContextDeclarations(py::module &);
void initFontDeclarations(py::module &);
void initImageDeclarations(py::module &);
void initImageInfoDeclarations(py::module &);
void initMatrixDeclarations(py::module &);
void initPaintDeclarations(py::module &);
void initPathDeclarations(py::module &);
void initPathMeasureDeclarations(py::module &);
void initPictureDeclarations(py::module &);
void initPixmapDeclarations(py::module &);
void initPointDeclarations(py::module &);
void initRectDeclarations(py::module &);
void initRefCntDeclarations(py::module &);
void initRegionDeclarations(py::module &);
void initSamplingOptionsDeclarations(py::module &);
void initScalarDeclarations(py::module &);
void initSizeDeclarations(py::module &);
void initStreamDeclarations(py::module &);
void initStringDeclarations(py::module &);
void initSurfaceDeclarations(py::module &);
void initTextBlobDeclarations(py::module &);
void initVerticesDeclarations(py::module &);
void initSVGDOMDeclarations(py::module &);

void initBitmapDefinitions(py::module &);
void initBlendModeDefinitions(py::module &);
void initCanvasDefinitions(py::module &);
void initColorDefinitions(py::module &);
void initCodecDefinitions(py::module &);
void initColorSpaceDefinitions(py::module &);
void initDataDefinitions(py::module &);
void initDocumentDefinitions(py::module &);
void initGrContextDefinitions(py::module &);
void initFontDefinitions(py::module &);
void initImageDefinitions(py::module &);
void initImageInfoDefinitions(py::module &);
void initMatrixDefinitions(py::module &);
void initPaintDefinitions(py::module &);
void initPathDefinitions(py::module &);
void initPathMeasureDefinitions(py::module &);
void initPictureDefinitions(py::module &);
void initPixmapDefinitions(py::module &);
void initPointDefinitions(py::module &);
void initRectDefinitions(py::module &);
void initRefCntDefinitions(py::module &);
void initRegionDefinitions(py::module &);
void initSamplingOptionsDefinitions(py::module &);
void initScalarDefinitions(py::module &);
void initSizeDefinitions(py::module &);
void initStreamDefinitions(py::module &);
void initStringDefinitions(py::module &);
void initSurfaceDefinitions(py::module &);
void initTextBlobDefinitions(py::module &);
void initVerticesDefinitions(py::module &);
void initSVGDOMDefinitions(py::module &);

// Main entry point.
PYBIND11_MODULE(skia, m) {
    m.doc() = R"docstring(
    Python Skia binding module.
    )docstring";

    // Declarations.
    initRefCntDeclarations(m);

    initBlendModeDeclarations(m);
    initColorDeclarations(m);
    initColorSpaceDeclarations(m);
    initSizeDeclarations(m);
    initPointDeclarations(m);
    initRectDeclarations(m);
    initRegionDeclarations(m);
    initMatrixDeclarations(m);
    initDataDeclarations(m);
    initStreamDeclarations(m);
    initStringDeclarations(m);
    initSamplingOptionsDeclarations(m); // Before Image and Canvas

    initCodecDeclarations(m);
    initBitmapDeclarations(m);
    initDocumentDeclarations(m);
    initFontDeclarations(m);
    initGrContextDeclarations(m);
    initImageInfoDeclarations(m);
    initImageDeclarations(m);
    initPaintDeclarations(m);
    initPathDeclarations(m);
    initPathMeasureDeclarations(m);
    initPictureDeclarations(m);
    initPixmapDeclarations(m);
    initScalarDeclarations(m);
    initTextBlobDeclarations(m);
    initVerticesDeclarations(m);

    initCanvasDeclarations(m);
    initSurfaceDeclarations(m);
    initSVGDOMDeclarations(m);

    // Definitions.
    initRefCntDefinitions(m);

    initBlendModeDefinitions(m);
    initColorDefinitions(m);
    initColorSpaceDefinitions(m);
    initSizeDefinitions(m);
    initPointDefinitions(m);
    initRectDefinitions(m);
    initRegionDefinitions(m);
    initMatrixDefinitions(m);
    initDataDefinitions(m);
    initStreamDefinitions(m);
    initStringDefinitions(m);
    initSamplingOptionsDefinitions(m); // Before Image and Canvas

    initCodecDefinitions(m);
    initBitmapDefinitions(m);
    initDocumentDefinitions(m);
    initFontDefinitions(m);
    initGrContextDefinitions(m);
    initImageInfoDefinitions(m);
    initImageDefinitions(m);
    initPaintDefinitions(m);
    initPathDefinitions(m);
    initPathMeasureDefinitions(m);
    initPictureDefinitions(m);
    initPixmapDefinitions(m);
    initScalarDefinitions(m);
    initTextBlobDefinitions(m);
    initVerticesDefinitions(m);

    initCanvasDefinitions(m);
    initSurfaceDefinitions(m);
    initSVGDOMDefinitions(m);

    SkGraphics::SetOpenTypeSVGDecoderFactory(SkSVGOpenTypeSVGDecoder::Make);

#ifdef VERSION_INFO
    m.attr("__version__") = XSTRING(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
