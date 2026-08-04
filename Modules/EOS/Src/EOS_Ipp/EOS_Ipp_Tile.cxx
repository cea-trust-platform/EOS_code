/****************************************************************************
 * Copyright (c) 2023, CEA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

#include "EOS_Ipp_Tile.hxx"
#include "EOS_Ipp.hxx"

namespace NEPTUNE_EOS
{
  EOS_Ipp_Tile::~EOS_Ipp_Tile()
  {
    delete ipp_;
  }

  bool EOS_Ipp_Tile::ensure_loaded(const std::string &interpolation_suffix)
  {
    if (ipp_ != nullptr)
      return true;

    EOS_Ipp *candidate = new EOS_Ipp();
    const EOS_Error err = candidate->load_from_med_path(AString(desc_.med_file.c_str()));
    if (err != EOS_Error::good && err != EOS_Error::ok)
    {
      delete candidate;
      return false;
    }

    // load_from_med_path() does not parse a ":bicubic"/":bilinear" suffix (it
    // is not given one -- the tile's file path is used as-is), so the
    // interpolation method selected on the outer, tiled-mode EOS_Ipp is
    // applied explicitly here instead, once the tile is loaded.
    if (interpolation_suffix == "bicubic")
      candidate->set_interpolation_method(EOS_Ipp::BICUBIC);
    else if (interpolation_suffix == "bilinear")
      candidate->set_interpolation_method(EOS_Ipp::BILINEAR);

    ipp_ = candidate;
    return true;
  }
}
