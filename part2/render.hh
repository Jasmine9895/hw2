#if !defined (INC_RENDER_HH)
#define INC_RENDER_HH

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
//#define DEBUG
//#define DEBUG_EXTENDED
namespace gil = boost::gil;

struct Color_Point {
  int r, g, b;
};
/** Construct a color suitable for display. */
gil::rgb8_pixel_t render(float v);

Color_Point render_color(float v);
#endif
