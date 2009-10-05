// Filename: PPInstance.h
// Created by:  atrestman (14Sept09)
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

#pragma once

#include <string>
#include <vector>
#include <math.h>
#include "afxmt.h"

#include "p3d_plugin.h"
#include "PPDownloadCallback.h"
#include "PPLogger.h"
#include "fileSpec.h"

#define WM_PY_LAUNCHED        (WM_USER + 1)
#define WM_PROGRESS           (WM_USER + 2)
#define WM_PANDA_NOTIFICATION (WM_USER + 3)

class CP3DActiveXCtrl;

extern P3D_instance_feed_url_stream_func *P3D_instance_feed_url_stream;
extern P3D_make_class_definition_func *P3D_make_class_definition;
extern P3D_new_undefined_object_func *P3D_new_undefined_object;
extern P3D_new_none_object_func *P3D_new_none_object;
extern P3D_new_bool_object_func *P3D_new_bool_object;
extern P3D_new_int_object_func *P3D_new_int_object;
extern P3D_new_float_object_func *P3D_new_float_object;
extern P3D_new_string_object_func *P3D_new_string_object;

class PPInstance
{
public:
    PPInstance( CP3DActiveXCtrl& parentCtrl );
    virtual ~PPInstance( );

    int DownloadP3DComponents( std::string& p3dDllFilename );

    int LoadPlugin( const std::string& dllFilename );
    int UnloadPlugin( void );

    int Start( const std::string& p3dFileName );

    std::string GetHostUrl( );
    std::string GetP3DFilename( );

    static void HandleRequestLoop();

    inline bool IsInit() { return m_isInit; }

    HWND m_parentWnd;
    CEvent m_eventStop;

    P3D_object* m_p3dObject;

protected:
    PPInstance( );
    PPInstance( const PPInstance& );

    int DownloadFile( const std::string& from, const std::string& to );
    int CopyFile( const std::string& from, const std::string& to );

    bool read_contents_file(const std::string &contents_filename);
    void read_xhost(TiXmlElement *xhost);
    void add_mirror(std::string mirror_url);
    void choose_random_mirrors(std::vector<std::string> &result, int num_mirrors);

    void HandleRequest( P3D_request *request );
    static void HandleRequestGetUrl( void *data );

    P3D_instance* m_p3dInstance;
    CP3DActiveXCtrl& m_parentCtrl;
    PPLogger m_logger;

    bool m_handleRequestOnUIThread;
    bool m_isInit;
    bool m_pluginLoaded;

    std::string _download_url_prefix;
    typedef std::vector<std::string> Mirrors;
    Mirrors _mirrors;
    FileSpec _core_api_dll;

    std::string m_rootDir;
};
