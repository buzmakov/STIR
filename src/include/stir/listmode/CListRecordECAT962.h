//
// $Id$
//
/*!

  \file
  \ingroup listmode
  \brief Classes for listmode events for the ECAT 962 (aka Exact HR+)
    
  \author Kris Thielemans
      
  $Date$
  $Revision$
*/
/*
    Copyright (C) 2000- $Date$, IRSL
    See STIR/LICENSE.txt for details
*/

#ifndef __stir_listmode_CListRecordECAT962_H__
#define __stir_listmode_CListRecordECAT962_H__

#include "local/stir/listmode/CListRecord.h"
#include "stir/ProjDataInfoCylindrical.h"
#include "stir/ProjDataInfoCylindricalNoArcCorr.h"
#include "stir/ByteOrderDefine.h"
#include "stir/round.h"


#ifndef STIR_NO_NAMESPACES
using std::istream;
#endif

START_NAMESPACE_STIR
class Bin;
class ProjDataInfoCylindrical;

#define STIRListModeFileFormatIsBigEndian

//typedef unsigned int            BITS;

//! Class for storing and using a coincidence event from a listmode file
/*! The private definition is specific to the 962. Public members are generic
    though.

  For the 962 the event word is 32 bit. To save 1 bit in size, a 2d sinogram
     encoding is used (as opposed to a detector number on the ring
     for both events).
     Both bin and view use 9 bits, so their maximum range is
     512 values, which is fine for the 962 (which needs only 288).

  \todo use DetectionPosition etc.
*/
class CListEventDataECAT962 
{
 public:  
  inline bool is_prompt() const { return random == 0; }
  inline void set_prompt(const bool prompt = true) 
    { if (prompt) random=0; else random=1;}

  //! This routine returns the corresponding detector pair   
  void get_detectors(
		   int& det_num_a, int& det_num_b, int& ring_a, int& ring_b) const;

/*! This routine constructs a (prompt) coincidence event */
  void set_detectors(
			const int det_num_a, const int det_num_b,
			const int ring_a, const int ring_b);

/*! This routine returns the corresponding tangential_pos_num,view_num,ring_a and ring_b
   */
  void get_sinogram_and_ring_coordinates(int& view, int& tangential_pos_num, int& ring_a, int& ring_b) const;
  
/*! This routine constructs a coincidence event */
  void set_sinogram_and_ring_coordinates(
			const int view_num, const int tangential_pos_num, 
			const int ring_a, const int ring_b);

  void get_bin(Bin&, const ProjDataInfoCylindrical&) const;


 private:
  const static int num_views;

    /* ring encoding. use as follows:
       This organisation corresponds to physical detector blocks (which
       have 8 crystal rings). Names are not very good probably...
       */				
    /* 'random' bit:
        1 if event is Random (it fell in delayed time window) */
    /* bin field  is shifted in a funny way, use the following code to find
       bin_number:
         if ( bin > NumProjBinsBy2 ) bin -= NumProjBins ;
	 */

#ifdef STIRByteOrderIsBigEndian
  unsigned    type    : 1; /* 0-coincidence event, 1-time tick */
  unsigned    block_A_ring_bit1 : 1;
  unsigned    block_B_ring_bit1 : 1;
  unsigned    block_A_ring_bit0 : 1;
  unsigned    block_B_ring_bit0 : 1;
  unsigned    block_B_detector : 3;
  unsigned    block_A_detector : 3;
  unsigned    scatter  : 1;
  unsigned    random  : 1;
  unsigned    multiple  : 1;
  unsigned    bin : 9;
  unsigned    view : 9;
#else
  // Do byteswapping first before using this bit field.
  unsigned    view : 9;
  unsigned    bin : 9;
  unsigned    multiple  : 1;
  unsigned    random  : 1;
  unsigned    scatter  : 1;
  unsigned    block_A_detector : 3;
  unsigned    block_B_detector : 3;
  unsigned    block_B_ring_bit0 : 1;
  unsigned    block_A_ring_bit0 : 1;
  unsigned    block_B_ring_bit1 : 1;
  unsigned    block_A_ring_bit1 : 1;
  unsigned    type    : 1; /* 0-coincidence event, 1-time tick */

#endif
}; /*-coincidence event*/

class CListRecordECAT962;

//! A class for storing and using a timing 'event' from a listmode file
class CListTimeDataECAT962
{
 public:
  inline double get_time_in_secs() const
  { return time/1000.;  }
  inline void set_time_in_secs(const double time_in_secs)
  { time = ((1U<<28)-1) & static_cast<unsigned>(round(time_in_secs * 1000)); }
  inline unsigned int get_gating() const
  { return gating; }
  inline void set_gating(unsigned int g)
    { gating = g & 0xf;} // TODO not sure if this is correct
private:
  friend class CListRecordECAT962; // to give access to type field
#ifdef STIRByteOrderIsBigEndian
  unsigned    type : 1;    /* 0-coincidence event, 1-time tick */
  unsigned    gating : 4;  /* some info about the gating signals */
  unsigned    time : 27 ;  /* since scan start */
#else
  // Do byteswapping first before using this bit field.
  unsigned    time : 27 ;  /* since scan start */
  unsigned    gating : 4;  /* some info about the gating signals */
  unsigned    type : 1;    /* 0-coincidence event, 1-time tick */
#endif
};

//! A class for a general element of a listmode file
/*! For the 962 it's either a coincidence event, or a timing flag.*/
  class CListRecordECAT962 : public CListRecord, public CListTime, public CListEvent
{
private:
  static shared_ptr<Scanner> 
    scanner_sptr;

  static shared_ptr<ProjDataInfoCylindricalNoArcCorr>
    uncompressed_proj_data_info_sptr;

public:

  bool is_time() const
  { return time_data.type == 1U; }
  bool is_event() const
  { return time_data.type == 0U; }
  virtual CListEvent&  event() 
    { return *this; }
  virtual const CListEvent&  event() const
    { return *this; }
  virtual CListTime&   time()
    { return *this; }
  virtual const CListTime&   time() const
    { return *this; }

  bool operator==(const CListRecord& e2) const
  {
    return dynamic_cast<CListRecordECAT962 const *>(&e2) != 0 &&
      raw == static_cast<CListRecordECAT962 const &>(e2).raw;
  }	    
  virtual char const * get_const_data_ptr() const
    { return reinterpret_cast<char const *>(&raw); }
  virtual char * get_data_ptr() 
    { return reinterpret_cast<char *>(&raw); }

  // time 
  inline double get_time_in_secs() const 
    { return time_data.get_time_in_secs(); }
  inline void set_time_in_secs(const double time_in_secs)
    { time_data.set_time_in_secs(time_in_secs); }
  inline unsigned int get_gating() const
    { return time_data.get_gating(); }
  inline void set_gating(unsigned int g) 
    { time_data.set_gating(g); }

  // event
  inline bool is_prompt() const { return event_data.is_prompt(); }
  inline void set_prompt(const bool prompt = true) { event_data.set_prompt(prompt); }
  void get_bin(Bin& bin, const ProjDataInfo& proj_data_info) const 
    {
      event_data.get_bin(bin, dynamic_cast<const ProjDataInfoCylindrical&>(proj_data_info));
    }
  void get_uncompressed_bin(Bin& bin) const;

  shared_ptr<ProjDataInfoCylindricalNoArcCorr>
    get_uncompressed_proj_data_info_sptr() const
    { return uncompressed_proj_data_info_sptr; }

  // private:
  union {
    CListEventDataECAT962  event_data;
    CListTimeDataECAT962   time_data; 
    long         raw;
  };

};



END_NAMESPACE_STIR

#endif
