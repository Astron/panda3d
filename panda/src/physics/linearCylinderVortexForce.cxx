// Filename: LinearCylinderVortexForce.cxx
// Created by:  charles (24Jul00)
// 
////////////////////////////////////////////////////////////////////

#include "config_physics.h"
#include "linearCylinderVortexForce.h"

TypeHandle LinearCylinderVortexForce::_type_handle;

////////////////////////////////////////////////////////////////////
//    Function : LinearCylinderVortexForce
//      Access : public
// Description : Simple Constructor
////////////////////////////////////////////////////////////////////
LinearCylinderVortexForce::
LinearCylinderVortexForce(float radius, float length, float coef, 
		    float a, bool md) :
  LinearForce(a, md),
  _radius(radius), _length(length), _coef(coef)
{
}

////////////////////////////////////////////////////////////////////
//    Function : LinearCylinderVortexForce
//      Access : public
// Description : copy Constructor
////////////////////////////////////////////////////////////////////
LinearCylinderVortexForce::
LinearCylinderVortexForce(const LinearCylinderVortexForce &copy) :
  LinearForce(copy) {
  _radius = copy._radius;
  _length = copy._length;
  _coef = copy._coef;
}

////////////////////////////////////////////////////////////////////
//    Function : ~LinearCylinderVortexForce
//      Access : public
// Description : Destructor
////////////////////////////////////////////////////////////////////
LinearCylinderVortexForce::
~LinearCylinderVortexForce(void) {
}

////////////////////////////////////////////////////////////////////
//    Function : make_copy
//      Access : public, virtual
// Description : child copier
////////////////////////////////////////////////////////////////////
LinearForce *LinearCylinderVortexForce::
make_copy(void) {
  return new LinearCylinderVortexForce(*this);
}

////////////////////////////////////////////////////////////////////
//    Function : get_child_vector
//      Access : private, virtual
// Description : returns the centripetal force vector for the
//               passed-in object
////////////////////////////////////////////////////////////////////
LVector3f LinearCylinderVortexForce::
get_child_vector(const PhysicsObject *po) {
  // get the force-space transform- this MUST be the relative matrix
  // from the point's local coordinate system to the attached node's
  // local system.
  //  LMatrix4f force_space_xform = LMatrix4f::ident_mat();
  LVector3f force_vec(0.0f, 0.0f, 0.0f);

  // project the point into force_space
  LPoint3f point = po->get_position();

  // clip along length
  if (point[2] < 0.0f || point[2] > _length)
    return force_vec;

  // clip to radius
  float x_squared = point[0] * point[0];
  float y_squared = point[1] * point[1];
  float dist_squared = x_squared + y_squared;
  float radius_squared = _radius * _radius;

  // squared space increases monotonically wrt linear space,
  // so there's no need to sqrt to check inside/outside this disc.
  if (dist_squared > radius_squared)
    return force_vec;

  float r = sqrtf(dist_squared);

  LVector3f tangential = po->get_velocity();
  tangential[2] = 0.0f;

  LVector3f centripetal = -point;
  centripetal[2] = 0.0f;
  centripetal.normalize();

  //  a = v^2 / r
  centripetal = centripetal * _coef * (tangential.length_squared() / r);

  return centripetal;
}
