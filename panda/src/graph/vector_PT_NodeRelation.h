// Filename: vector_PT_NodeRelation.h
// Created by:  drose (07May01)
// 
////////////////////////////////////////////////////////////////////

#ifndef VECTOR_PT_NODERELATION_H
#define VECTOR_PT_NODERELATION_H

#include <pandabase.h>

#include "nodeRelation.h"
#include "pt_NodeRelation.h"

#include <vector>

////////////////////////////////////////////////////////////////////
//       Class : vector_PT_NodeRelation
// Description : A vector of PT(NodeRelation)'s.  This class is defined once
//               here, and exported to PANDA.DLL; other packages that
//               want to use a vector of this type (whether they need
//               to export it or not) should include this header file,
//               rather than defining the vector again.
////////////////////////////////////////////////////////////////////

#ifdef HAVE_DINKUM
#define VV_PT_NODERELATION std::_Vector_val<PT_NodeRelation, std::allocator<PT_NodeRelation> >
EXPORT_TEMPLATE_CLASS(EXPCL_PANDA, EXPTP_PANDA, VV_PT_NODERELATION)
#endif
EXPORT_TEMPLATE_CLASS(EXPCL_PANDA, EXPTP_PANDA, std::vector<PT_NodeRelation>)
typedef vector<PT_NodeRelation> vector_PT_NodeRelation;

// Tell GCC that we'll take care of the instantiation explicitly here.
#ifdef __GNUC__
#pragma interface
#endif

#endif
