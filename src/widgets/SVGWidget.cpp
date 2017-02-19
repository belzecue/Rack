#include "widgets.hpp"


// #define DEBUG_ONLY(x) x
#define DEBUG_ONLY(x)

namespace rack {


static NVGcolor getNVGColor(uint32_t color) {
	return nvgRGBA(
		(color >> 0) & 0xff,
		(color >> 8) & 0xff,
		(color >> 16) & 0xff,
		(color >> 24) & 0xff);
}

static void drawSVG(NVGcontext *vg, NSVGimage *svg) {
	DEBUG_ONLY(printf("new image: %g x %g px\n", svg->width, svg->height);)
	int shapeIndex = 0;
	for (NSVGshape *shape = svg->shapes; shape; shape = shape->next, shapeIndex++) {
		DEBUG_ONLY(printf("	new shape: %d id \"%s\", fillrule %d\n", shapeIndex, shape->id, shape->fillRule);)

		if (!(shape->flags & NSVG_FLAGS_VISIBLE))
			continue;

		nvgSave(vg);

		if (shape->opacity < 1.0)
			nvgGlobalAlpha(vg, shape->opacity);

		nvgStrokeWidth(vg, shape->strokeWidth);
		// strokeDashOffset, strokeDashArray, strokeDashCount not yet supported
		// strokeLineJoin, strokeLineCap not yet supported

		// Build path
		nvgBeginPath(vg);
		for (NSVGpath *path = shape->paths; path; path = path->next) {
			DEBUG_ONLY(printf("		new path: %d points, %s\n", path->npts, path->closed ? "closed" : "open");)

			nvgMoveTo(vg, path->pts[0], path->pts[1]);
			for (int i = 1; i < path->npts; i += 3) {
				float *p = &path->pts[2*i];
				nvgBezierTo(vg, p[0], p[1], p[2], p[3], p[4], p[5]);
				// nvgLineTo(vg, p[4], p[5]);
			}

			if (path->closed)
				nvgClosePath(vg);

			if (path->next)
				nvgPathWinding(vg, NVG_HOLE);
		}

		// Fill shape
		if (shape->fill.type) {
			switch (shape->fill.type) {
				case NSVG_PAINT_COLOR: {
					NVGcolor color = getNVGColor(shape->fill.color);
					nvgFillColor(vg, color);
					DEBUG_ONLY(printf("		fill color (%g, %g, %g, %g)\n", color.r, color.g, color.b, color.a);)
				} break;
				case NSVG_PAINT_LINEAR_GRADIENT: {
					// NSVGgradient *g = shape->fill.gradient;
					// printf("		lin grad: %f\t%f\n", g->fx, g->fy);
				} break;
			}
			nvgFill(vg);
		}

		// Stroke shape
		if (shape->stroke.type) {
			switch (shape->stroke.type) {
				case NSVG_PAINT_COLOR: {
					NVGcolor color = getNVGColor(shape->stroke.color);
					nvgStrokeColor(vg, color);
					DEBUG_ONLY(printf("		stroke color (%g, %g, %g, %g)\n", color.r, color.g, color.b, color.a);)
				} break;
				case NSVG_PAINT_LINEAR_GRADIENT: {
					// NSVGgradient *g = shape->stroke.gradient;
					// printf("		lin grad: %f\t%f\n", g->fx, g->fy);
				} break;
			}
			nvgStroke(vg);
		}

		nvgRestore(vg);
	}

	DEBUG_ONLY(printf("\n");)
}


void SVGWidget::wrap() {
	if (svg)
		box.size = Vec(svg->handle->width, svg->handle->height);
	else
		box.size = Vec();
}

void SVGWidget::draw(NVGcontext *vg) {
	if (svg && svg->handle) {
		drawSVG(vg, svg->handle);
	}
}


} // namespace rack
