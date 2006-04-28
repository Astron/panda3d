// Filename: mouseWatcherGroup.h
// Created by:  drose (02Jul01)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#ifndef MOUSEWATCHERGROUP_H
#define MOUSEWATCHERGROUP_H

#include "pandabase.h"
#include "mouseWatcherRegion.h"

#include "pointerTo.h"
#include "referenceCount.h"
#include "pvector.h"
#include "nodePath.h"
#include "pmutex.h"

////////////////////////////////////////////////////////////////////
//       Class : MouseWatcherGroup
// Description : This represents a collection of MouseWatcherRegions
//               that may be managed as a group.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA MouseWatcherGroup : virtual public ReferenceCount {
public:
  MouseWatcherGroup();
  virtual ~MouseWatcherGroup();

PUBLISHED:
  void add_region(MouseWatcherRegion *region);
  bool has_region(MouseWatcherRegion *region) const;
  bool remove_region(MouseWatcherRegion *region);
  MouseWatcherRegion *find_region(const string &name) const;
  void clear_regions();

  int get_num_regions() const;
  MouseWatcherRegion *get_region(int n) const;

  void output(ostream &out) const;
  void write(ostream &out, int indent_level = 0) const;

#ifndef NDEBUG
  void show_regions(const NodePath &render2d);
  void set_color(const Colorf &color);
  void hide_regions();
#endif  // NDEBUG

protected:
  bool do_remove_region(MouseWatcherRegion *region);

#ifndef NDEBUG
  virtual void do_show_regions(const NodePath &render2d);
  virtual void do_hide_regions();
#endif  // NDEBUG

protected:
  typedef pvector< PT(MouseWatcherRegion) > Regions;
  Regions _regions;

  // This mutex protects the above list of regions, as well as the
  // below list of vizzes.  It is also referenced directly by
  // MouseWatcher, a derived class.
  Mutex _lock;

private:
#ifndef NDEBUG
  void update_regions();
  PandaNode *make_viz_region(MouseWatcherRegion *region);

  typedef pvector< PT(PandaNode) > Vizzes;
  Vizzes _vizzes;

  bool _show_regions;
  NodePath _show_regions_root;
  Colorf _color;
#endif  // NDEBUG

public:
  static TypeHandle get_class_type() {
    ReferenceCount::init_type();
    return _type_handle;
  }
  static void init_type() {
    register_type(_type_handle, "MouseWatcherGroup",
                  ReferenceCount::get_class_type());
  }

private:
  static TypeHandle _type_handle;

  friend class MouseWatcher;
  friend class BlobWatcher;
};

#endif
