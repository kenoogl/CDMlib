#ifndef _CIO_DFI_PLOT3D_H_
#define _CIO_DFI_PLOT3D_H_

/*
 * CDMlib - Cartesian Data Management library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   cio_DFI_PLOT3D.h
 * @brief  cio_DFI_PLOT3D Class Header
 * @author aics    
 */

#include "cio_DFI.h"

class cio_DFI_PLOT3D : public cio_DFI {

protected:

  bool m_OutputGrid;  ///< plot3d grid file 出力指示

public:

  /** コンストラクタ */
  cio_DFI_PLOT3D();

  /** 
   * @brief コンストラクタ 
   * @param [in] F_Info  FileInfo
   * @param [in] F_Path  FilePath
   * @param [in] unit    Unit
   * @param [in] domain  Domain
   * @param [in] mpi     MPI
   * @param [in] TSlice  TimeSlice
   * @param [in] process Process
   */
  cio_DFI_PLOT3D(const cio_FileInfo F_Info, 
                 const cio_FilePath F_Path, 
                 const cio_Unit unit, 
                 const cio_Domain domain, 
                 const cio_MPI mpi,
                 const cio_TimeSlice TSlice, 
                 const cio_Process process)
  {
    DFI_Finfo      = F_Info; 
    DFI_Fpath      = F_Path;
    DFI_Unit       = unit;
    DFI_Domain     = domain;
    DFI_MPI        = mpi;
    DFI_TimeSlice  = TSlice;
    DFI_Process    = process;
    m_OutputGrid   = true;
    m_bgrid_interp_flag = true;
  };
  
  /**　デストラクタ */
  ~cio_DFI_PLOT3D();

public:

protected:

  /**
   * @brief sphファイルのヘッダーレコード読込み
   * @param[in]  fp          ファイルポインタ
   * @param[in]  matchEndian エンディアンチェックフラグ true:合致
   * @param[in]  step        ステップ番号
   * @param[in]  head        dfiのHeadIndex
   * @param[in]  tail        dfiのTailIndex
   * @param[in]  gc          dfiのガイドセル数
   * @param[out] voxsize     voxsize
   * @param[out] time        時刻
   * @return error code
   */
  CIO::E_CIO_ERRORCODE
  read_HeaderRecord(FILE* fp, 
                    bool matchEndian,
                    unsigned step,
                    const int head[3],
                    const int tail[3],
                    int gc, 
                    int voxsize[3],
                    double &time)
  { return CIO::E_CIO_SUCCESS; };

  /**
   * @brief フィールドデータファイルのデータレコード読込み
   * @param[in]  fp          ファイルポインタ
   * @param[in]  matchEndian true:Endian一致
   * @param[in]  buf         読込み用バッファ
   * @param[in]  head        読込みバッファHeadIndex
   * @param[in]  nz          z方向のボクセルサイズ（実セル＋ガイドセル＊２）
   * @param[out] src         読み込んだデータを格納した配列のポインタ
   * @return error code
   */ 
  CIO::E_CIO_ERRORCODE
  read_Datarecord(FILE* fp,
                  bool matchEndian,
                  cio_Array* buf,
                  int head[3],
                  int nz,
                  cio_Array* &src)
  { return CIO::E_CIO_SUCCESS; };

  /**
   * @brief sphファイルのAverageデータレコードの読込み
   * @param[in]  fp          ファイルポインタ
   * @param[in]  matchEndian true:Endian一致
   * @param[in]  step        読込みstep番号
   * @param[out] avr_step    平均ステップ   
   * @param[out] avr_time    平均タイム
   * @return error code
   */
  CIO::E_CIO_ERRORCODE
  read_averaged(FILE* fp,
                bool matchEndian,
                unsigned step, 
                unsigned &avr_step,
                double &avr_time) 
  { return CIO::E_CIO_SUCCESS; };

  /**
   * @brief SPHヘッダファイルの出力
   * @param[in] fp     ファイルポインタ
   * @param[in] step   ステップ番号
   * @param[in] time   時刻
   * @param[in] RankID ランク番号
   * @return error code
   */
  CIO::E_CIO_ERRORCODE
  write_HeaderRecord(FILE* fp, 
                     const unsigned step, 
                     const double time, 
                     const int RankID); 

  /**
   * @brief SPHデータレコードの出力
   * @param[in]  fp ファイルポインタ
   * @param[in]  val データポインタ
   * @param[in]  gc ガイドセル
   * @param[in]  RankID ランク番号
   * @return error code
   */
  CIO::E_CIO_ERRORCODE
  write_DataRecord(FILE* fp, 
                   cio_Array* val, 
                   const int gc, 
                   const int RankID); 

  /**
   * @brief Averageレコードの出力
   * @param[in] fp       ファイルポインタ
   * @param[in] step_avr 平均ステップ番号
   * @param[in] time_avr 平均時刻
   * @return error code
   */
  CIO::E_CIO_ERRORCODE
  write_averaged(FILE* fp,
                 const unsigned step_avr,
                 const double time_avr) 
  { return CIO::E_CIO_SUCCESS; };

  /**
   * @brief Grid data file 出力 コントロール
   */
  bool
  write_GridData(); 

  /**
   * @brief xyzを計算して出力
   * @param [in] fp  出力ファイルポインタ
   * @param [in] org 原点座標値
   * @param [in] pit ピッチ
   * @param [in] sz  サイズ
   */ 
  template<class T>
  void
  write_XYZ(FILE* fp, T* org, T* pit, int sz[3]);

  /**
   * @brief func data 出力
   * @param[in] fp    出力ファイルポインタ
   * @param[in] data  出力データポインタ
   * @param[in] sz    出力データのサイズ
   * @param[in] ncomp 出力成分数
   */ 
  template<class T>
  void write_Func(FILE* fp, cio_TypeArray<T>* data, const int sz[3], int ncomp);

};


//inline 関数
#include "inline/cio_Plot3d_inline.h"


#endif // _cio_DFI_PLOT3D_H_
