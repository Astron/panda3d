// Filename: config_audio.h
// Created by:  cary (22Sep00)
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

#ifndef __CONFIG_AUDIO_H__
#define __CONFIG_AUDIO_H__

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableInt.h"
#include "configVariableBool.h"
#include "configVariableDouble.h"
#include "configVariableString.h"
#include "configVariableFilename.h"

#ifdef WIN32_VC
#pragma warning (disable : 4231)
#endif

NotifyCategoryDecl(audio, EXPCL_PANDA, EXPTP_PANDA);

//We Need This one.
extern EXPCL_PANDA ConfigVariableString audio_library_name;

// Config vars for Fmod:

extern EXPCL_PANDA ConfigVariableInt fmod_number_of_sound_channels;
extern EXPCL_PANDA ConfigVariableBool fmod_use_surround_sound;

// Config vars for Miles:

extern EXPCL_PANDA ConfigVariableBool audio_software_midi;
extern EXPCL_PANDA ConfigVariableFilename audio_dls_file;
extern EXPCL_PANDA ConfigVariableInt audio_output_rate;
extern EXPCL_PANDA ConfigVariableInt audio_output_bits;
extern EXPCL_PANDA ConfigVariableInt audio_output_channels;



#ifndef NDEBUG //[
  // Non-release build:
  #define audio_debug(msg) \
  if (audio_cat.is_debug()) { \
    audio_cat->debug() << msg << endl; \
  } else {}

  #define audio_info(msg) \
    audio_cat->info() << msg << endl

  #define audio_warning(msg) \
    audio_cat->warning() << msg << endl
#else //][
  // Release build:
  #define audio_debug(msg) ((void)0)
  #define audio_info(msg) ((void)0)
  #define audio_warning(msg) ((void)0)
#endif //]

#define audio_error(msg) \
  audio_cat->error() << msg << endl

#endif /* __CONFIG_AUDIO_H__ */
