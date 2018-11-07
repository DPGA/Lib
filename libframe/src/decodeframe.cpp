/*! \mainpage Documentation de la librairie de décode de trame DPGA
 * 
 * \section intro_sec Introduction
 *
 * Cette librairie à pour but de fournir un certain nombre de fonction contenu dans une classe.<br>
 * Ces fonctions permettent de recupérer toutes les informations des trames provenant de carte AMC 
 *
 * \section Download_sec	Download
 * 
 * "svn svn+ssh://user_name@svn.in2p3.fr/dpga/Soft/FirmwareTests/ServeurUdp"
 * 
 * \section Compilation_sec Compilation
 * 
 * il faut le programme cmake version >= 2.8 <br>
 * cd build <br>
 * cmake .. <br>
 * make <br>
 * 
 * \section install_sec Installation
 *
 * la librairie libframe.a est dans le répertoire build/lib<br>
 * le programme testframe est dans le répertoire build/bin
 * 
 * 
 * \section copyright Copyright and License
 * Laboraroite de Physique de Clermont-Ferrand PLUS
 * \image html logo1.jpg
 * 
 *
 * <BR><BR>
 *
 */

/**
 * \file decodeframe.cpp
 * \brief Library to decode Frame DPGA
 * \author Daniel Lambert
 * \version 0.1
 * \date 10/207/2017
 * 
 * Library to decode Frame from the DPGA
 * 
 */


#include <string.h>
#include <assert.h>
#include "decodeframe.h"

#define DECSOC(a) ((a & 0xff00) >> 8)

std::string getVersionDecodeFrame() {return VERSION_DECODEFRAME;}

DecodeFrame::DecodeFrame(const char* filename)
{
	HeaderFile = NULL;
	HeaderFrame = NULL;
	SetFile(filename);
  
}

DecodeFrame::DecodeFrame()
{
  HeaderFile = (struct S_HeaderFile *) malloc(sizeof(S_HeaderFile));
  memset(&ErrFrame,0,sizeof(struct S_ErrorFrame));
  EraseAlloc	= false;
  HeaderFrame = NULL;
}

//**********************************************

bool DecodeFrame::SetFile_Write(const char* filename)
{
  HeaderFile = (struct S_HeaderFile *) malloc(sizeof(S_HeaderFile));
  HeaderFrame = (struct S_HeaderFrame *) malloc(sizeof(S_HeaderFrame));
  EraseAlloc	= true;
  FileName = filename;
  File = fopen(filename,"r");
  if (File == NULL) {
    printf("Error openning file %s\n",filename);
    return false;
  }
  else {
    
  }
  return(true);
}


bool DecodeFrame::SetPacket_Write(uint16_t *Packet)
{
  HeaderFrame = (struct S_HeaderFrame *) Packet;
  HeaderFile->FrontEndId = GetFeId();
  HeaderFile->NbSamples = GetNbSamples();
  pChannel = &Packet[(sizeof(struct S_HeaderFrame) >> 1)];
  
  Soc = DECSOC(pChannel[0]);
  Crc = ntohs(pChannel[(HeaderFile->NbSamples+2)*4]);
  EoF = ntohs(pChannel[(HeaderFile->NbSamples+2)*4+1]);
  Index = 0;
  return true;
}

//**************************************************

bool DecodeFrame::SetFile(const char* filename)
{
	
  if (HeaderFile == NULL) 	HeaderFile = (struct S_HeaderFile *) malloc(sizeof(S_HeaderFile));
  if (HeaderFrame == NULL) HeaderFrame = (struct S_HeaderFrame *) malloc(sizeof(S_HeaderFrame));
  EraseAlloc	= true;
  FileName = filename;
  File = fopen(filename,"r");
  if (File == NULL) {
    printf("Error openning file %s\n",filename);
    return false;
  }
  else {
    fread(HeaderFile,1,sizeof(S_HeaderFile),File);
    printf("Mode file = %d, Feid = %x NbSample= %d\n",HeaderFile->ModeFile,HeaderFile->FrontEndId,HeaderFile->NbSamples);
    pChannel = (uint16_t *) calloc(4,(HeaderFile->NbSamples*2+8));  // 4 Channels
    if (!pChannel) {
      printf("Error allocating memory pChannel\n"); 
      return(false);
    }
  }
  return(true);
}



S_HeaderFile *DecodeFrame::GetHeaderFile()
{
  assert(File != NULL);
  return (HeaderFile);
}

bool DecodeFrame::SetPacket(uint16_t *Packet)
{
  HeaderFrame = (struct S_HeaderFrame *) Packet;
  HeaderFile->FrontEndId = GetFeId();
  HeaderFile->NbSamples = GetNbSamples();
  pChannel = &Packet[(sizeof(struct S_HeaderFrame) >> 1)];
  
  Soc = DECSOC(pChannel[0]);
  Crc = ntohs(pChannel[(HeaderFile->NbSamples+2)*4]);
  EoF = ntohs(pChannel[(HeaderFile->NbSamples+2)*4+1]);
  Index = 0;
  return true;
}


bool DecodeFrame::SetPacket(uint16_t *Packet,uint16_t lg)
{
  HeaderFrame = (struct S_HeaderFrame *) Packet;
  HeaderFile->FrontEndId = GetFeId();
  HeaderFile->NbSamples = GetNbSamples();
  pChannel = &Packet[(sizeof(struct S_HeaderFrame) >> 1)];
  
  Soc = DECSOC(pChannel[0]);
  if (((HeaderFile->NbSamples+2)*4) <= lg) {
    Crc = ntohs(pChannel[(HeaderFile->NbSamples+2)*4]);
    EoF = ntohs(pChannel[(HeaderFile->NbSamples+2)*4+1]);
  }
  else {
    Crc = 0x00;
    EoF = 0x00;
  }
  Index = 0;
  return true;
}

bool DecodeFrame::FrameError()
{
  if (!IsSoFOk()) ErrFrame.ErrSoF++;
  if (!IsCafeDecaOk()) ErrFrame.ErrCafeDeca ++;
  if (!IsBoboOk()) ErrFrame.ErrBobo++;
  //if (!IsSoCOk()) ErrFrame.ErrSoc++; //pas implementer car si extrait que le header Soc n'est pas lu
  if (!IsEoFOk()) ErrFrame.ErrEoF++;
  if (!IsCrcOk()) ErrFrame.ErrCrc++;
  if (IsErrorTT()) ErrFrame.ErrTT++;
  return (IsSoFOk() & IsCafeDecaOk() & IsBoboOk() & IsEoFOk() & IsCrcOk() & !IsErrorTT());
}

bool DecodeFrame::FrameErrornoTT()
{
  if (!IsSoFOk()) ErrFrame.ErrSoF++;
  if (!IsCafeDecaOk()) ErrFrame.ErrCafeDeca ++;
  if (!IsBoboOk()) ErrFrame.ErrBobo++;
  //if (!IsSoCOk()) ErrFrame.ErrSoc++; //pas implementer car si extrait que le header Soc n'est pas lu
  if (!IsEoFOk()) ErrFrame.ErrEoF++;
  if (!IsCrcOk()) ErrFrame.ErrCrc++;
  return (IsSoFOk() & IsCafeDecaOk() & IsBoboOk() & IsEoFOk() & IsCrcOk());
}


DecodeFrame::~DecodeFrame()
{
  if ((pChannel) && (EraseAlloc))		free(pChannel);
  if ((HeaderFrame) && (EraseAlloc)) 	free(HeaderFrame);
  if (HeaderFile) 	free(HeaderFile);
}

S_HeaderFrame *DecodeFrame::GetHeaderFrame()
{
  return HeaderFrame;
}

void DecodeFrame::Print(uint16_t *val,uint16_t n)
{
  for (int i=0;i<n;++i) {
    printf("%04x ",ntohs(val[i]));
    if (((i+1)%16) == 0) printf("\n");
  }
  printf("\n");
}

uint16_t *DecodeFrame::NextPacket()
{
  Index = 0;
  // 	pChannel=NULL;
  // 	HeaderFrame=NULL;
  
  // 	if (!feof(File)) { 
  // 		fread(HeaderFrame,1,sizeof(S_HeaderFrame),File);
  // 		fread(pChannel,1,(HeaderFile->NbSamples*2+4)*4+4,File);
  // 		Crc = ntohs(pChannel[(HeaderFile->NbSamples+2)*4]);
  // 		EoF = ntohs(pChannel[(HeaderFile->NbSamples+2)*4+1]);
  // 		return(pChannel);
  // 	}
  // 	else{ 
  // 	  printf("End of File %s \n",FileName);
  // 	  return(NULL);
  // 	  
  // 	}
  
  if (!feof(File)) { 
    size_t result;
    fread(HeaderFrame,1,sizeof(S_HeaderFrame),File);
    
    result= fread(pChannel,1,(HeaderFile->NbSamples*2+4)*4+4,File);
    if (result != (size_t) ( (HeaderFile->NbSamples*2+4)*4+4)) {/*fputs ("Reading error\n",stderr);*/ return (NULL);}
    
    Crc = ntohs(pChannel[(HeaderFile->NbSamples+2)*4]);
    EoF = ntohs(pChannel[(HeaderFile->NbSamples+2)*4+1]);
    return(pChannel);
  }
  else{ 
    printf("End of File %s \n",FileName);
    return(NULL);
  }
  
  return(NULL);
}


// uint16_t *DecodeFrame::CopypChannel()
// {
// 	
// 	if (pChannel) { 
// 		fread(HeaderFrame,1,sizeof(S_HeaderFrame),File);
// 		fread(pChannel,1,(HeaderFile->NbSamples*2+4)*4+4,File);
// 		
// 	}
// 	else{ 
// 	  return(NULL);
// 	}
// 	return(pChannel);
// }



uint16_t *DecodeFrame::NextChannel(uint16_t *PacketChannel,const bool withChannel)
{
  
  if (Index > (HeaderFile->NbSamples+2)*4) {
    printf("reinit index %d\n",Index);
    Index=0;
  }
  if (PacketChannel != NULL) {
    uint16_t *p = &PacketChannel[Index];
    Soc			= DECSOC(p[0]);
    curChannel 	= p[0] & 0x7f;
    curSrout   	= ntohs(p[1]) & 0x3ff;
    if (!withChannel) p += 2;  // remove Channel Number and Srout
    
    Index += HeaderFile->NbSamples+2; // +2 channel + srout
    
    return (p);
  }
  else return (NULL);
}


uint16_t * DecodeFrame::GetChannel(int ch)
{
  int addr= (HeaderFile->NbSamples+2)*ch;
  
  Soc			= DECSOC(pChannel[addr]);
  curChannel 	= pChannel[addr] & 0x7f;
  curSrout   	= ntohs(pChannel[addr+1]) & 0x3ff;
  return(&pChannel[(HeaderFile->NbSamples+2)*ch]);
  
}

void DecodeFrame::ExtractHeader()
{
  assert(File != NULL);
  uint16_t *p;
  TstpAsm.clear();
  TstpThor.clear();
  TstpThorAsm.clear();
  Pattern.clear();
  TTError.clear();
  memset(&ErrFrame,0,sizeof(struct S_ErrorFrame));
  while ((p=NextPacket()) != NULL) {
    FrameError();
    TstpAsm.push_back(GetTimeStpAsm());
    TstpThor.push_back(GetTimeStpThor());
    TstpThorAsm.push_back(GetTimeStpThorAsm());
    Pattern.push_back(GetPattern());
    TTError.push_back(IsErrorTT());
  }
  
}

bool DecodeFrame::SaveChannel(const char *name,uint16_t * p)
{
  char *temp = (char *) malloc(strlen(name)+10);
  sprintf(temp,"%s-%x-ch%d",name,GetFeId(),curChannel);
  FILE *f=fopen(temp,"a");
  free(temp);
  if (!f) return false;
  else {
    uint16_t Nb = ntohs(HeaderFile->NbSamples);	// Probleme d'inversion voir à la lecture du fichier
    printf("Nb=%d\n",Nb);
    fwrite(&Nb,2,1,f); // à la place de 2 size of uint16_t
    fwrite(p,1,(HeaderFile->NbSamples*2+4),f);  //+4 pour channel + srout Attention appeller NextChannel avec withChannel = true
    
    // *2 pour octect 
    fclose(f);
  }
  return true;
  
}
