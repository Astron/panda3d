// Filename: qptrackball.h
// Created by:  drose (12Mar02)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://www.panda3d.org/license.txt .
//
// To contact the maintainers of this program write to
// panda3d@yahoogroups.com .
//
////////////////////////////////////////////////////////////////////

#ifndef qpTRACKBALL_H
#define qpTRACKBALL_H

#include "pandabase.h"

#include "qpdataNode.h"
#include "qpnodePath.h"
#include "modifierButtons.h"
#include "luse.h"
#include "linmath_events.h"


////////////////////////////////////////////////////////////////////
//       Class : qpTrackball
// Description : Trackball acts like Performer in trackball mode.  It
//               can either spin around a piece of geometry directly,
//               or it can spin around a camera with the inverse
//               transform to make it appear that the whole world is
//               spinning.
//
//               The Trackball object actually just places a transform
//               in the data graph; parent a Transform2SG node under
//               it to actually transform objects (or cameras) in the
//               world.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA qpTrackball : public qpDataNode {
PUBLISHED:
  qpTrackball(const string &name);
  ~qpTrackball();

  void reset();

  /// **** Translation ****

  const LPoint3f &get_pos() const;
  float get_x() const;
  float get_y() const;
  float get_z() const;
  void set_pos(const LVecBase3f &vec);
  void set_pos(float x, float y, float z);
  void set_x(float x);
  void set_y(float y);
  void set_z(float z);

  /// **** Rotation ****

  LVecBase3f get_hpr() const;
  float get_h() const;
  float get_p() const;
  float get_r() const;
  void set_hpr(const LVecBase3f &hpr);
  void set_hpr(float h, float p, float r);
  void set_h(float h);
  void set_p(float p);
  void set_r(float r);

  /// **** Origin of Rotation ****

  void reset_origin_here();
  void move_origin(float x, float y, float z);

  /// **** Misc ****

  void set_invert(bool flag);
  bool get_invert() const;

  void set_rel_to(const qpNodePath &_rel_to);
  const qpNodePath &get_rel_to() const;

  void set_coordinate_system(CoordinateSystem cs);
  CoordinateSystem get_coordinate_system() const;

  void set_mat(const LMatrix4f &mat);
  const LMatrix4f &get_mat() const;
  const LMatrix4f &get_trans_mat() const;


private:
  void apply(double x, double y, int button);

  void reextract();
  void recompute();


  float _lastx, _lasty;

  float _rotscale;
  float _fwdscale;

  LMatrix4f _rotation;
  LPoint3f _translation;
  LMatrix4f _mat, _orig;
  bool _invert;
  qpNodePath _rel_to;
  CoordinateSystem _cs;

  // Remember which mouse buttons are being held down.
  ModifierButtons _mods;


protected:
  // Inherited from DataNode
  virtual void do_transmit_data(const DataNodeTransmit &input,
                                DataNodeTransmit &output);

private:
  // inputs
  int _pixel_xy_input;
  int _button_events_input;

  // outputs
  int _transform_output;

  PT(EventStoreMat4) _transform;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    qpDataNode::init_type();
    register_type(_type_handle, "qpTrackball",
                  qpDataNode::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

#endif
