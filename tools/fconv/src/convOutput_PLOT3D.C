/*
 * fconv (File Converter)
 *
 * CDMlib - Cartesian Data Management library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/**
 * @file   convOutput_PLOT3D.C
 * @brief  convOutput_PLOT3D Class
 * @author aics
 */

#include "convOutput.h"
#include "convOutput_PLOT3D.h"

// #################################################################
// コンストラクタ
convOutput_PLOT3D::convOutput_PLOT3D()
{


}

// #################################################################
// デストラクタ
convOutput_PLOT3D::~convOutput_PLOT3D()
{


}

// #################################################################
// grid 出力
void convOutput_PLOT3D::WriteGridData(std::string prefix,
                                      int step,
                                      int myRank,
                                      int dType,
                                      int guide,
                                      double org[3],
                                      double pit[3],
                                      int sz[3])
{

  //step 0 以外は出力しない
  if( step != 0 ) return; 

  int id,jd,kd;
  id=sz[0]+1;
  jd=sz[1]+1;
  kd=sz[2]+1;
  size_t maxsize = (size_t)id*(size_t)jd*(size_t)kd*3;
  size_t outsize = (size_t)id*(size_t)jd*(size_t)kd;

  int outDtype = m_InputCntl->Get_OutputDataType();
  if( outDtype == CIO::E_CIO_DTYPE_UNKNOWN ) outDtype = dType;

  if( outDtype == CIO::E_CIO_FLOAT64 ) {
    double* x = new double[maxsize];
    OutputPlot3D_xyz(prefix, step, myRank, guide, org, pit, sz, &x[0], 
                     &x[outsize] , &x[outsize*2] );
  }else if( outDtype == CIO::E_CIO_FLOAT32 ) {
    float* x = new float[maxsize];
    OutputPlot3D_xyz(prefix, step, myRank, guide, org, pit, sz, &x[0], 
                     &x[outsize] , &x[outsize*2] );
  }
}

// #################################################################
// グリッド数の 出力
void convOutput_PLOT3D::WriteNgrid(FILE* fp, int ngrid)
{
  switch (m_InputCntl->Get_OutputFormatType()) {
    case CIO::E_CIO_OUTPUT_TYPE_FBINARY:
      unsigned int dmy;
      dmy = sizeof(int);
      WriteDataMarker(dmy,fp,true);
      fwrite(&ngrid, sizeof(int), 1, fp);
      WriteDataMarker(dmy,fp,true);
      break;
    case CIO::E_CIO_OUTPUT_TYPE_ASCII:
      fprintf(fp,"%5d\n",ngrid);
      break;
    case CIO::E_CIO_OUTPUT_TYPE_BINARY:
      fwrite(&ngrid, sizeof(int), 1, fp);
      break;      
    default:
      break;
    }
}

// #################################################################
// ブロックデータの 出力
void convOutput_PLOT3D::WriteBlockData(FILE* fp, int id, int jd, int kd)
{

  switch (m_InputCntl->Get_OutputFormatType()) {
    case CIO::E_CIO_OUTPUT_TYPE_FBINARY:
      unsigned int dmy;
      dmy = sizeof(int)*3;
      WriteDataMarker(dmy,fp,true);
      fwrite(&id, sizeof(int), 1, fp);
      fwrite(&jd, sizeof(int), 1, fp);
      fwrite(&kd, sizeof(int), 1, fp);
      WriteDataMarker(dmy,fp,true);
      break;
    case CIO::E_CIO_OUTPUT_TYPE_ASCII:
      fprintf(fp,"%5d%5d%5d\n",id,jd,kd);
      break;
    case CIO::E_CIO_OUTPUT_TYPE_BINARY:
      fwrite(&id, sizeof(int), 1, fp);
      fwrite(&jd, sizeof(int), 1, fp);
      fwrite(&kd, sizeof(int), 1, fp);
      break;
   default:
      break;
   }      

}

// #################################################################
// フィールドデータ出力ファイルオープン
FILE* convOutput_PLOT3D::OutputFile_Open(
                         const std::string prefix,
                         const unsigned step,
                         const int id,
                         const bool mio)
{
  FILE* fp;

  //ファイル名の生成
  std::string outfile;
  CIO::E_CIO_OUTPUT_FNAME fnameformat = m_InputCntl->Get_OutputFilenameFormat();
  outfile = m_InputCntl->Get_OutputDir() +"/"+
            cio_DFI::Generate_FileName(prefix,
                                       id,
                                       step,
                                       "func",
                                       fnameformat,
                                       mio,
                                       CIO::E_CIO_OFF);

  //出力ファイルオープン
  // ascii
  if( m_InputCntl->Get_OutputFormatType() == CIO::E_CIO_OUTPUT_TYPE_ASCII ) {
    if( (fp = fopen(outfile.c_str(), "wa")) == NULL ) {
      printf("\tCan't open file.(%s)\n",outfile.c_str());
      Exit(0);
    }
  } else {
  //binary
    if( (fp = fopen(outfile.c_str(), "wb")) == NULL ) {
      printf("\tCan't open file.(%s)\n",outfile.c_str());
      Exit(0);
    }
  }

  return fp;

}

// #################################################################
// ヘッダーレコードを出力
bool convOutput_PLOT3D::WriteHeaderRecord(
                                       int step,
                                       int dim,
                                       CIO::E_CIO_DTYPE out_type,
                                       int imax,
                                       int jmax,
                                       int kmax,
                                       double time,
                                       double* org,
                                       double* pit,
                                       std::string prefix,
                                       FILE *fp)
{
  if( !fp ) return false;

  //ngirdの初期化
  int ngrid=1;

  //ngrid の出力
  WriteNgrid(fp,ngrid);

  //block data の出力
  
  WriteFuncBlockData(fp,imax+1,jmax+1,kmax+1,dim);

  return true;

}



// #################################################################
// func 出力
bool convOutput_PLOT3D::WriteFieldData(FILE* fp, 
                                       cio_Array* src, 
                                       size_t dLen)
{

  const int* sz = src->getArraySizeInt();

  cio_Array *out = cio_Array::instanceArray
                   (src->getDataType(),
                    src->getArrayShape(),
                    (int *)sz,
                    0,
                    src->getNcomp());

  int ret = src->copyArray(out);

  WriteFuncData(fp, out);
  delete out;

  return true;

}

// #################################################################
// func ブロックデータの 出力
void convOutput_PLOT3D::WriteFuncBlockData(FILE* fp, int id, int jd, int kd, int nvar)
{

  switch (m_InputCntl->Get_OutputFormatType()) {
    case CIO::E_CIO_OUTPUT_TYPE_FBINARY:
      unsigned int dmy;
      dmy = sizeof(int)*4;
      WriteDataMarker(dmy,fp,true);
      fwrite(&id, sizeof(int), 1, fp);
      fwrite(&jd, sizeof(int), 1, fp);
      fwrite(&kd, sizeof(int), 1, fp);
      fwrite(&nvar, sizeof(int), 1, fp);
      WriteDataMarker(dmy,fp,true);
      break;
    case CIO::E_CIO_OUTPUT_TYPE_ASCII:
      fprintf(fp,"%5d%5d%5d%5d\n",id,jd,kd,nvar);
      break;
    case CIO::E_CIO_OUTPUT_TYPE_BINARY:
      fwrite(&id, sizeof(int), 1, fp);
      fwrite(&jd, sizeof(int), 1, fp);
      fwrite(&kd, sizeof(int), 1, fp);
      fwrite(&nvar, sizeof(int), 1, fp);
      break;
    default:
      break;
  }

}

// #################################################################
void convOutput_PLOT3D::WriteFuncData(FILE* fp, cio_Array* p3src)
{

  const int* sz = p3src->getArraySizeInt();
  size_t dLen = (size_t)sz[0]*(size_t)sz[1]*(size_t)sz[2]*p3src->getNcomp();

  switch (m_InputCntl->Get_OutputFormatType()) {
    case CIO::E_CIO_OUTPUT_TYPE_FBINARY:
      unsigned int dmy;
      if( p3src->getDataType() == CIO::E_CIO_FLOAT32 ) {
        dmy = sizeof(float)*dLen;
      } else {
        dmy = sizeof(double)*dLen;
      }
      WriteDataMarker(dmy,fp,true);
      p3src->writeBinary(fp);      
      WriteDataMarker(dmy,fp,true);
      break;
    case CIO::E_CIO_OUTPUT_TYPE_ASCII:
      if( p3src->getDataType() == CIO::E_CIO_FLOAT32) {
        float *data = (float*)p3src->getData();
        for(int i=0; i<dLen; i++) fprintf(fp,"%15.6E\n",data[i]);
      } else if( p3src->getDataType() == CIO::E_CIO_FLOAT64) {
        double *data = (double*)p3src->getData();
        for(int i=0; i<dLen; i++) fprintf(fp,"%15.6E\n",data[i]);
      }
      break;
    case CIO::E_CIO_OUTPUT_TYPE_BINARY:
      p3src->writeBinary(fp);
      break;
    default:
      break;
  }
    
}

// #################################################################
//
bool convOutput_PLOT3D::WriteDataMarker(int dmy, FILE* fp, bool out)
{
  if( !out ) return true;
  if( m_InputCntl->Get_OutputFormatType() != CIO::E_CIO_OUTPUT_TYPE_FBINARY ) return true;
  if( fwrite(&dmy, sizeof(int), 1, fp) != 1 ) return false;
  return true;
}
                                         
