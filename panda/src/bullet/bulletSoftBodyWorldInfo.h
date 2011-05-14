// Filename: bulletSoftBodyWorldInfo.h
// Created by:  enn0x (04Mar10)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#ifndef __BULLET_SOFT_BODY_WORLD_INFO_H__
#define __BULLET_SOFT_BODY_WORLD_INFO_H__

#include "pandabase.h"

#include "bullet_includes.h"
#include "bullet_utils.h"

#include "lvector3.h"

////////////////////////////////////////////////////////////////////
//       Class : BulletSoftBodyWorldInfo
// Description : 
////////////////////////////////////////////////////////////////////
class EXPCL_PANDABULLET BulletSoftBodyWorldInfo {

PUBLISHED:
  INLINE ~BulletSoftBodyWorldInfo();

  void set_air_density(float density);
  void set_water_density(float density);
  void set_water_offset(float offset);
  void set_water_normal(const LVector3f &normal);
  void set_gravity(const LVector3f &gravity);

  float get_air_density() const;
  float get_water_density() const;
  float get_water_offset() const;
  LVector3f get_water_normal() const;
  LVector3f get_gravity() const;

  void garbage_collect(int lifetime=256);

public:
  BulletSoftBodyWorldInfo(btSoftBodyWorldInfo &_info);

  INLINE btSoftBodyWorldInfo &get_info() const;

private:
  btSoftBodyWorldInfo &_info;
};

#include "bulletSoftBodyWorldInfo.I"

#endif // __BULLET_SOFT_BODY_WORLD_INFO_H__
