// Filename: cLwoSurface.cxx
// Created by:  drose (25Apr01)
// 
////////////////////////////////////////////////////////////////////

#include "cLwoSurface.h"
#include "cLwoSurfaceBlock.h"
#include "cLwoClip.h"
#include "lwoToEggConverter.h"

#include <lwoSurfaceColor.h>
#include <lwoSurfaceParameter.h>
#include <lwoSurfaceSmoothingAngle.h>
#include <lwoSurfaceSidedness.h>
#include <lwoSurfaceBlock.h>
#include <eggPrimitive.h>
#include <string_utils.h>
#include <mathNumbers.h>


////////////////////////////////////////////////////////////////////
//     Function: CLwoSurface::Constructor
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
CLwoSurface::
CLwoSurface(LwoToEggConverter *converter, const LwoSurface *surface) :
  _converter(converter),
  _surface(surface)
{
  _flags = 0;
  _checked_texture = false;
  _has_uvs = false;
  _map_uvs = NULL;
  _block = (CLwoSurfaceBlock *)NULL;

  // Walk through the chunk list, looking for some basic properties.
  int num_chunks = _surface->get_num_chunks();
  for (int i = 0; i < num_chunks; i++) {
    const IffChunk *chunk = _surface->get_chunk(i);

    if (chunk->is_of_type(LwoSurfaceColor::get_class_type())) {
      const LwoSurfaceColor *color = DCAST(LwoSurfaceColor, chunk);
      _flags |= F_color;
      _color = color->_color;

    } else if (chunk->is_of_type(LwoSurfaceParameter::get_class_type())) {
      const LwoSurfaceParameter *param = DCAST(LwoSurfaceParameter, chunk);
      IffId type = param->get_id();

      if (type == IffId("DIFF")) {
	_flags |= F_diffuse;
	_diffuse = param->_value;

      } else if (type == IffId("LUMI")) {
	_flags |= F_luminosity;
	_luminosity = param->_value;

      } else if (type == IffId("SPEC")) {
	_flags |= F_specular;
	_specular = param->_value;

      } else if (type == IffId("REFL")) {
	_flags |= F_reflection;
	_reflection = param->_value;

      } else if (type == IffId("TRAN")) {
	_flags |= F_transparency;
	_transparency = param->_value;

      } else if (type == IffId("TRNL")) {
	_flags |= F_translucency;
	_translucency = param->_value;
      }

    } else if (chunk->is_of_type(LwoSurfaceSmoothingAngle::get_class_type())) {
      const LwoSurfaceSmoothingAngle *sa = DCAST(LwoSurfaceSmoothingAngle, chunk);
      _flags |= F_smooth_angle;
      _smooth_angle = sa->_angle;

    } else if (chunk->is_of_type(LwoSurfaceSidedness::get_class_type())) {
      const LwoSurfaceSidedness *sn = DCAST(LwoSurfaceSidedness, chunk);
      _flags |= F_backface;
      _backface = (sn->_sidedness == LwoSurfaceSidedness::S_front_and_back);

    } else if (chunk->is_of_type(LwoSurfaceBlock::get_class_type())) {
      const LwoSurfaceBlock *lwo_block = DCAST(LwoSurfaceBlock, chunk);
      // One of possibly several blocks in the texture that define
      // additional fancy rendering properties.

      CLwoSurfaceBlock *block = new CLwoSurfaceBlock(_converter, lwo_block);

      // We only consider enabled "IMAP" type blocks that affect "COLR".
      if (block->_block_type == IffId("IMAP") &&
	  block->_channel_id == IffId("COLR") &&
	  block->_enabled) {
	// Now save the block with the lowest ordinal.
	if (_block == (CLwoSurfaceBlock *)NULL) {
	  _block = block;

	} else if (block->_ordinal < _block->_ordinal) {
	  delete _block;
	  _block = block;

	} else {
	  delete block;
	}

      } else {
	delete block;
      }
    }      
  }
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoSurface::Destructor
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
CLwoSurface::
~CLwoSurface() {
  if (_block != (CLwoSurfaceBlock *)NULL) {
    delete _block;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoSurface::apply_properties
//       Access: Public
//  Description: Applies the color, texture, etc. described by the
//               surface to the indicated egg primitive.
//
//               If the surface defines a smoothing angle,
//               smooth_angle may be updated to reflect it if the
//               angle is greater than that specified.
////////////////////////////////////////////////////////////////////
void CLwoSurface::
apply_properties(EggPrimitive *egg_prim, vector_PT_EggVertex &egg_vertices,
		 float &smooth_angle) {
  if (!_surface->_source.empty()) {
    // This surface is derived from another surface; apply that one
    // first.
    CLwoSurface *parent = _converter->get_surface(_surface->_source);
    if (parent != (CLwoSurface *)NULL && parent != this) {
      parent->apply_properties(egg_prim, egg_vertices, smooth_angle);
    }
  }

  // We treat color and transparency separately, because Lightwave
  // does, and this will allow us to treat inherited surfaces a little
  // more robustly.
  if ((_flags & F_color) != 0) {
    Colorf color(1.0, 1.0, 1.0, 1.0);
    if (egg_prim->has_color()) {
      color = egg_prim->get_color();
    }

    color[0] = _color[0];
    color[1] = _color[1];
    color[2] = _color[2];

    egg_prim->set_color(color);
  }

  if (check_texture()) {
    // Texture overrides the primitive's natural color.
    egg_prim->set_texture(_egg_texture);
    egg_prim->clear_color();

    // Assign UV's to the vertices.
    generate_uvs(egg_vertices);
  }

  if ((_flags & F_transparency) != 0) {
    Colorf color(1.0, 1.0, 1.0, 1.0);
    if (egg_prim->has_color()) {
      color = egg_prim->get_color();
    }

    color[3] = 1.0 - _transparency;
    egg_prim->set_color(color);
  }

  if ((_flags & F_backface) != 0) {
    egg_prim->set_bface_flag(_backface);
  }

  if ((_flags & F_smooth_angle) != 0) {
    smooth_angle = max(smooth_angle, _smooth_angle);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoSurface::check_texture
//       Access: Public
//  Description: Checks whether the surface demands a texture or not.
//               Returns true if so, false otherwise.
//
//               If the surface demands a texture, this also sets up
//               _egg_texture and _compute_uvs as appropriate for the
//               texture.
////////////////////////////////////////////////////////////////////
bool CLwoSurface::
check_texture() {
  if (_checked_texture) {
    return (_egg_texture != (EggTexture *)NULL);
  }
  _checked_texture = true;
  _egg_texture = (EggTexture *)NULL;
  _has_uvs = false;
  _map_uvs = NULL;

  if (_block == (CLwoSurfaceBlock *)NULL) {
    // No texture.  Not even a shader block.
    return false;
  }

  int clip_index = _block->_clip_index;
  if (clip_index < 0) {
    // No image file associated with the texture.
    return false;
  }

  CLwoClip *clip = _converter->get_clip(clip_index);
  if (clip == (CLwoClip *)NULL) {
    nout << "No clip image with index " << clip_index << "\n";
    return false;
  }

  if (!clip->is_still_image()) {
    // Can't do anything with an animated image right now.
    return false;
  }

  Filename pathname = _converter->convert_texture_path(clip->_filename);

  _egg_texture = new EggTexture("clip" + format_string(clip_index), pathname);

  // Do we need to generate UV's?
  switch (_block->_projection_mode) {
  case LwoSurfaceBlockProjection::M_planar:
    _has_uvs = true;
    _map_uvs = &CLwoSurface::map_planar;
    break;

  case LwoSurfaceBlockProjection::M_cylindrical:
    _has_uvs = true;
    _map_uvs = &CLwoSurface::map_cylindrical;
    break;

  case LwoSurfaceBlockProjection::M_spherical:
    _has_uvs = true;
    _map_uvs = &CLwoSurface::map_spherical;
    break;

  case LwoSurfaceBlockProjection::M_cubic:
    _has_uvs = true;
    _map_uvs = &CLwoSurface::map_cubic;
    break;

  case LwoSurfaceBlockProjection::M_front:
    // Cannot generate "front" UV's, since this depends on a camera.
    // Is it supposed to be updated in real time, like a projected
    // texture?
    break;

  case LwoSurfaceBlockProjection::M_uv:
    // "uv" projection means to use the existing UV's already defined
    // for the vertex.  This case was already handled in the code that
    // created the EggVertex pointers.
    break;
  };

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoSurface::generate_uvs
//       Access: Private
//  Description: Computes all the UV's for the polygon's vertices,
//               according to the _projection_mode defined in the
//               block.
////////////////////////////////////////////////////////////////////
void CLwoSurface::
generate_uvs(vector_PT_EggVertex &egg_vertices) {
  nassertv(_map_uvs != NULL);

  // To do this properly near seams and singularities (for instance,
  // the back seam and the poles of the spherical map), we will need
  // to know the polygon's centroid.
  LPoint3d centroid(0.0, 0.0, 0.0);
  
  vector_PT_EggVertex::const_iterator vi;
  for (vi = egg_vertices.begin(); vi != egg_vertices.end(); ++vi) {
    EggVertex *egg_vertex = (*vi);
    centroid += egg_vertex->get_pos3();
  }

  centroid /= (double)egg_vertices.size();

  // Now go back through and actually compute the UV's.
  for (vi = egg_vertices.begin(); vi != egg_vertices.end(); ++vi) {
    EggVertex *egg_vertex = (*vi);
    LPoint3d pos = egg_vertex->get_pos3();
    LPoint2d uv = (this->*_map_uvs)(pos, centroid);
    egg_vertex->set_uv(uv);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoSurface::map_planar
//       Access: Private
//  Description: Computes a UV based on the given point in space,
//               using a planar projection.
////////////////////////////////////////////////////////////////////
LPoint2d CLwoSurface::
map_planar(const LPoint3d &pos, const LPoint3d &) const {
  // A planar projection is about as easy as can be.  We ignore the Y
  // axis, and project the point into the XZ plane.  Done.
  double u = (pos[0] + 0.5);
  double v = (pos[2] + 0.5);

  return LPoint2d(u, v);
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoSurface::map_spherical
//       Access: Private
//  Description: Computes a UV based on the given point in space,
//               using a spherical projection.
////////////////////////////////////////////////////////////////////
LPoint2d CLwoSurface::
map_spherical(const LPoint3d &pos, const LPoint3d &centroid) const {
  // To compute the x position on the frame, we only need to consider
  // the angle of the vector about the Y axis.  Project the vector
  // into the XZ plane to do this.

  LVector2d xz_orig(pos[0], pos[2]);
  LVector2d xz = xz_orig;
  double u_offset = 0.0;

  if (xz == LVector2d::zero()) {
    // If we have a point on either pole, we've got problems.  This
    // point maps to the entire bottom edge of the image, so which U
    // value should we choose?  It does make a difference, especially
    // if we have a number of polygons around the south pole that all
    // share the common vertex.

    // We choose the U value based on the polygon's centroid.
    xz.set(centroid[0], centroid[2]);

  } else if (xz[1] >= 0.0 && ((xz[0] < 0.0) != (centroid[0] < 0.))) {
    // Now, if our polygon crosses the seam along the back of the
    // sphere--that is, the point is on the back of the sphere (xz[1]
    // >= 0.0) and not on the same side of the XZ plane as the
    // centroid, we've got problems too.  We need to add an offset to
    // the computed U value, either 1 or -1, to keep all the vertices
    // of the polygon on the same side of the seam.

    u_offset = (xz[0] < 0.0) ? 1.0 : -1.0;
  }

  // The U value is based on the longitude: the angle about the Y
  // axis.
  double u = 
    (atan2(xz[0], -xz[1]) / (2.0 * MathNumbers::pi) + 0.5 + u_offset) * _block->_w_repeat;

  // Now rotate the vector into the YZ plane, and the V value is based
  // on the latitude: the angle about the X axis.
  LVector2d yz(pos[1], xz_orig.length());
  double v = 
    (atan2(yz[0], yz[1]) / MathNumbers::pi + 0.5) * _block->_h_repeat;

  return LPoint2d(u, v);
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoSurface::map_cylindrical
//       Access: Private
//  Description: Computes a UV based on the given point in space,
//               using a cylindrical projection.
////////////////////////////////////////////////////////////////////
LPoint2d CLwoSurface::
map_cylindrical(const LPoint3d &pos, const LPoint3d &centroid) const {
  // This is almost identical to the spherical projection, except for
  // the computation of V.

  LVector2d xz(pos[0], pos[2]);
  double u_offset = 0.0;

  if (xz == LVector2d::zero()) {
    // Although a cylindrical mapping does not really have a
    // singularity at the pole, it's still possible to put a point
    // there, and we'd like to do the right thing with the polygon
    // that shares that point.  So the singularity logic remains.
    xz.set(centroid[0], centroid[2]);

  } else if (xz[1] >= 0.0 && ((xz[0] < 0.0) != (centroid[0] < 0.))) {
    // And cylinders do still have a seam at the back.
    u_offset = (xz[0] < 0.0) ? 1.0 : -1.0;
  }

  double u = 
    (atan2(xz[0], -xz[1]) / (2.0 * MathNumbers::pi) + 0.5 + u_offset) * _block->_w_repeat;

  // For a cylindrical mapping, the V value comes almost directly from
  // Y.  Easy.
  double v = (pos[1] + 0.5);

  return LPoint2d(u, v);
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoSurface::map_cubic
//       Access: Private
//  Description: Computes a UV based on the given point in space,
//               using a cubic projection.
////////////////////////////////////////////////////////////////////
LPoint2d CLwoSurface::
map_cubic(const LPoint3d &pos, const LPoint3d &centroid) const {
  // A cubic projection is a planar projection, but we eliminate the
  // dominant axis (based on the polygon's centroid) instead of
  // arbitrarily eliminating Y.

  double x = fabs(centroid[0]);
  double y = fabs(centroid[1]);
  double z = fabs(centroid[2]);

  double u, v;
  
  if (x > y) {
    if (x > z) {
      // X is dominant.
      u = (pos[2] + 0.5);
      v = (pos[1] + 0.5);
    } else {
      // Z is dominant.
      u = (pos[0] + 0.5);
      v = (pos[1] + 0.5);
    }
  } else {
    if (y > z) {
      // Y is dominant.
      u = (pos[0] + 0.5);
      v = (pos[2] + 0.5);
    } else {
      // Z is dominant.
      u = (pos[0] + 0.5);
      v = (pos[1] + 0.5);
    }
  }

  return LPoint2d(u, v);
}
