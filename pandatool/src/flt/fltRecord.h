// filename: fltRecord.h
// Created by:  drose (24Aug00)
// 
////////////////////////////////////////////////////////////////////

#ifndef FLTRECORD_H
#define FLTRECORD_H

#include <pandatoolbase.h>

#include "fltOpcode.h"
#include "fltError.h"

#include <typeHandle.h>
#include <typedReferenceCount.h>
#include <pointerTo.h>

class FltHeader;
class FltRecordReader;
class FltRecordWriter;
class DatagramIterator;

////////////////////////////////////////////////////////////////////
// 	 Class : FltRecord
// Description : The base class for all kinds of records in a MultiGen
//               OpenFlight file.  A flt file consists of a hierarchy
//               of "beads" of various kinds, each of which may be
//               followed by n ancillary records, written sequentially
//               to the file.
////////////////////////////////////////////////////////////////////
class FltRecord : public TypedReferenceCount {
public:
  FltRecord(FltHeader *header);
  virtual ~FltRecord();

  int get_num_children() const;
  FltRecord *get_child(int n) const;
  void clear_children();
  void add_child(FltRecord *child);

  int get_num_subfaces() const;
  FltRecord *get_subface(int n) const;
  void clear_subfaces();
  void add_subface(FltRecord *subface);

  int get_num_extensions() const;
  FltRecord *get_extension(int n) const;
  void clear_extensions();
  void add_extension(FltRecord *extension);

  int get_num_ancillary() const;
  FltRecord *get_ancillary(int n) const;
  void clear_ancillary();
  void add_ancillary(FltRecord *ancillary);

  bool has_comment() const;
  const string &get_comment() const;
  void clear_comment();
  void set_comment(const string &comment);

  void check_remaining_size(const DatagramIterator &iterator) const;

  virtual void output(ostream &out) const;
  virtual void write(ostream &out, int indent_level = 0) const;

protected:
  void write_children(ostream &out, int indent_level) const;

  static bool is_ancillary(FltOpcode opcode);

  FltRecord *create_new_record(FltOpcode opcode) const;
  FltError read_record_and_children(FltRecordReader &reader);
  virtual bool extract_record(FltRecordReader &reader);
  virtual bool extract_ancillary(FltRecordReader &reader);

  virtual FltError write_record_and_children(FltRecordWriter &writer) const;
  virtual bool build_record(FltRecordWriter &writer) const;
  virtual FltError write_ancillary(FltRecordWriter &writer) const;

protected:
  FltHeader *_header;

private:
  typedef vector<PT(FltRecord)> Records;
  Records _children;
  Records _subfaces;
  Records _extensions;
  Records _ancillary;

  string _comment;


public:
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    TypedReferenceCount::init_type();
    register_type(_type_handle, "FltRecord",
		  TypedReferenceCount::get_class_type());
  }

private:
  static TypeHandle _type_handle;
};

INLINE ostream &operator << (ostream &out, const FltRecord &record);

#include "fltRecord.I"

#endif


