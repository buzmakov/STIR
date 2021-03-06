/*!
  \file
  \ingroup IO
  \brief Declaration of class stir::InputStreamFromROOTFile

  \author Nikos Efthimiou
  \author Harry Tsoumpas
*/
/*
 *  Copyright (C) 2015, 2016 University of Leeds
    Copyright (C) 2016, UCL
    This file is part of STIR.

    This file is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    See STIR/LICENSE.txt for details
*/

#ifndef __stir_IO_InputStreamFromROOTFile_H__
#define __stir_IO_InputStreamFromROOTFile_H__

#include "stir/shared_ptr.h"
#include "stir/Succeeded.h"
#include "stir/listmode/CListRecordROOT.h"
#include "stir/RegisteredObject.h"
#include "stir/ParsingObject.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TChain.h>
#include <TDirectory.h>
#include <TList.h>
#include <TChainElement.h>
#include <TTree.h>
#include <TFile.h>
#include <TVersionCheck.h>

START_NAMESPACE_STIR

//! A helper class to read data from a ROOT file Generated by GATE simulation toolkit
/*! \ingroup IO
        \author Nikos Efthimiou

        \details This class takes as input a root file, and returns the data stored in a meaningfull
        way. The validation of the ROOT input was done with version 5.34.
*/

class InputStreamFromROOTFile : public RegisteredObject< InputStreamFromROOTFile > ,
        public ParsingObject
{
public:
    typedef std::vector<long long int>::size_type SavedPosition;

    //! Default constructor
    InputStreamFromROOTFile();

    //! constructor
    InputStreamFromROOTFile(std::string filename,
                            std::string chain_name,
                            bool exclude_scattered, bool exclude_randoms,
                            float low_energy_window, float up_energy_window,
                            int offset_dets);


    virtual ~InputStreamFromROOTFile() {}

    //!
    //! \brief get_next_record
    //! \param record Reference to the Record
    //! \return
    //!  \details Returns the next record in the ROOT file.
    //!  The code is adapted from Sadek A. Nehmeh and CR Schmidtlein,
    //! downloaded from <a href="http://www.opengatecollaboration.org/STIR">GATE website</a>
    virtual
    Succeeded get_next_record(CListRecordROOT& record) = 0;
    //! Go to the first event.
    inline Succeeded reset();
    //! Save current position in a vector
    inline
    SavedPosition save_get_position();
    //! Set current position
    inline
    Succeeded set_get_position(const SavedPosition&);
    //! Get the vector with the saved positions
    inline
    std::vector<unsigned long int> get_saved_get_positions() const;
    //! Set a vector with saved positions
    inline
    void set_saved_get_positions(const std::vector<unsigned long int>& );
    //! Returns the total number of events
    inline virtual unsigned long int
    get_total_number_of_events() const;

    inline std::string get_ROOT_filename() const;

    //! Get the number of rings as calculated from the number of repeaters
    inline virtual int get_num_rings() const = 0;
    //! Get the number of dets per ring as calculated from the number of repeaters
    virtual int get_num_dets_per_ring() const = 0;
    //! Get the number of axial modules
    virtual int get_num_axial_blocks_per_bucket_v() const = 0;
    //! Get the number of transaxial modules
    virtual int get_num_transaxial_blocks_per_bucket_v() const = 0;
    //! Get the axial number of crystals per module
    virtual int get_num_axial_crystals_per_block_v() const = 0;
    //! Get the transaxial number of crystals per module
    virtual int get_num_transaxial_crystals_per_block_v() const = 0;

    virtual int get_num_axial_crystals_per_singles_unit() const = 0;

    virtual int get_num_trans_crystals_per_singles_unit() const = 0;

    inline virtual float get_low_energy_thres() const;

    inline virtual float get_up_energy_thres() const;

protected:

    virtual void set_defaults();
    virtual void initialise_keymap();
    virtual bool post_processing();

    //! Input data file name
    std::string filename;
    //! The starting position.
    unsigned long int starting_stream_position;
    //! The total number of entries
    unsigned long int nentries;
    //! Current get position
    unsigned long int current_position;
    //! A vector with saved position indices.
    std::vector<unsigned long int> saved_get_positions;

    // ROOT chain
    TChain *stream_ptr;

    // Variables to store root information
    std::string chain_name;
    Int_t           eventID1, eventID2;
    Double_t        time1, time2;
    Float_t         energy1, energy2;
    Int_t           comptonphantom1, comptonphantom2;

    bool exclude_scattered;
    bool exclude_randoms;

    float low_energy_window;
    float up_energy_window;
    int offset_dets;

    int singles_readout_depth;
};

END_NAMESPACE_STIR

#include "stir/IO/InputStreamFromROOTFile.inl"

#endif
