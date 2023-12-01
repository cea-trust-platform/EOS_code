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



/*
 * EOS_Med_i.hxx
 *
 *  Created on: 22 juin 2010
 */
#ifndef EOS_MED_I_HXX_
#define EOS_MED_I_HXX_
#define MESGERR 1

namespace NEPTUNE
{
  inline EOS_Error EOS_Med::create_File()
  { fid = MEDfileOpen(file.aschar() ,MED_ACC_CREAT) ;
    if (fid < 0)
       { cerr << "Error (create_File) : MEDfileOpen file=" << file.aschar() 
              <<" fid=" << fid << endl ;
         return EOS_Error::error ;
       }
    return EOS_Error::good ;
  }
  
  inline EOS_Error EOS_Med::rw_File()
  { EOS_Error err = check_file_format() ;
    if (err != good)  return err ;
  
    fid = MEDfileOpen(file.aschar() ,MED_ACC_RDWR) ;
    if (fid < 0)  return EOS_Error::error ;
  
    return EOS_Error::good ;
  }

  inline EOS_Error EOS_Med::read_File()
  { EOS_Error err = check_file_format() ;
    if (err != good)  return err ;
  
    fid = MEDfileOpen(file.aschar() ,MED_ACC_RDONLY) ;
    if (fid < 0)  return EOS_Error::error ;
  
    return EOS_Error::good ;
  }
  
  inline EOS_Error EOS_Med::close_File()
  { med_err ret = 0 ;
    if ((ret = MEDfileClose(fid)) < 0)  return EOS_Error::error ;
    return EOS_Error::good ;
  }
  
  inline EOS_Error EOS_Med::write_header(AString str)
  { /* write a header in file (max 200 characters) */
    char head[MED_COMMENT_SIZE+1] ;
    if (strlen(str.aschar()) >= MED_COMMENT_SIZE)
       { cerr << "Error ! Header length must be < 200 characters" << endl ;
         return EOS_Error::error ;
       }
  
    strcpy(head,str.aschar()) ;
  
    if (MEDfileCommentWr(fid,head) < 0)
       { cerr << "Error in writing header of the file !" << endl ;
         return EOS_Error::error ;
       }
  
    return EOS_Error::good ;
  }
  
  inline EOS_Error EOS_Med::read_header(AString& header)
  { med_err ret = 0 ;
    char desc[MED_COMMENT_SIZE+1] ;
  
    ret = MEDfileCommentRd(fid,desc) ;
    if (ret == 0)  
       header = desc ;
    else
       { cerr << "Erreur a la lecture de l'en-tete du fichier " << endl ;
         return EOS_Error::error ;
       }
    return EOS_Error::good ;
  }
  
  inline EOS_Error EOS_Med::check_file_format()
  { med_err ret = 0 ;
    med_bool hdfok ;
    med_bool medok ;
    ret = MEDfileCompatibility(file.aschar(), &hdfok, &medok) ;
    if (ret < 0) 
       { cerr << "File does not exist or it is impossible to read data in MED file" << endl ;
         return EOS_Error::error ;
       }
    else if (! hdfok) 
       { cerr << "Bad HDF Format" << endl ;
         return EOS_Error::error ;
       }
    else if (! medok) 
       { cerr << "Bad MED Format" << endl ;
         return EOS_Error::error ;
       }
  
    return EOS_Error::good ;
  }

  inline EOS_Error EOS_Med::get_entity_size(AString& m_name, int& size,
                  const med_entity_type entite, const med_geometry_type gemetrie,
                  const  med_data_type data, const med_connectivity_mode connectivite)
  { med_int size_med ;
    med_bool change, tranfo ;
    size_med = MEDmeshnEntity(fid,
                              m_name.aschar(),
                              MED_NO_DT,
                              MED_NO_IT,
                              entite,
                              gemetrie,
                              data,
                              connectivite,
                              &change,
                              &tranfo);
    if (size_med < 0)
       { cerr << "EOS_Med::get_entity_sizeErreur: Erreur a la lecture de la taille de l'entité du maillage" << m_name << endl ;
         return EOS_Error::error ;
       }
    size = size_med ;
  
    return EOS_Error::good ;
  }
  
  inline EOS_Error EOS_Med::get_number_champ(int& res)
  { med_int nchamps ;
  
    if ((nchamps = MEDnField(fid)) < 0)
       { cerr << "Can't read number of \"champs\"" << endl ;
         return EOS_Error::error ;
       }
    res = nchamps ;
  
    return EOS_Error::good ;
  }
  
  inline EOS_Error EOS_Med::get_Maillages_Infos(Strings& resname, ArrOfInt& resdim)
  { med_int nmaa, spacedim, meshdim, nstep ;
    char maa[MED_NAME_SIZE+1] ;
    char desc[MED_COMMENT_SIZE+1] ;
    char dtunit[MED_SNAME_SIZE+1] ;
    med_mesh_type meshtype ;
    med_sorting_type sortingtype ;
    med_axis_type axistype ;
    char  *axisname, *axisunit ;
  
    //number of MED mesh objects
    nmaa = MEDnMesh(fid) ;
    if (nmaa < 0) 
       { cerr << "Error, MED file reading : number of MED mesh objects" << endl ;
         return EOS_Error::error ;
       }
    resname.resize(nmaa) ;
    resdim.resize(nmaa)  ;
  
    //read med "maillage" object
    int i1, nbc_axis ;
    for (int i=0; i<nmaa; i++)
       { i1 = i + 1 ;
         if ( (spacedim = MEDmeshnAxis(fid, i1)) < 0 ) 
            { cerr << "Error, MED file reading : space dimension" << endl ;
              return EOS_Error::error ;
            }
         nbc_axis = (spacedim*MED_SNAME_SIZE) + 1 ;
         axisname = new char[nbc_axis] ;
         axisunit = new char[nbc_axis] ;

         if (MEDmeshInfo(fid, i1, maa, &spacedim, &meshdim, &meshtype, desc,
                         dtunit, &sortingtype, &nstep, &axistype, axisname, axisunit) < 0)
            { cerr << "Error, MED file reading : informations from MED mesh object n." << i  
                   << " / " << nmaa << endl ;
              delete[] axisname ;
              delete[] axisunit ;
              return EOS_Error::error ;
            }
         else
            { resname[i] = AString(maa) ;
              resdim[i]  = meshdim ;
              delete[] axisname ;
              delete[] axisunit ;
            }
       }
  
    return EOS_Error::good ;
  }

}
#endif /* EOS_MED_I_HXX_ */
