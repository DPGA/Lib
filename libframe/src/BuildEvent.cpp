/**
 * \brief Programme qui construit les event pour la librairie libframe <br>
 * \brief montre comment instantier la classe DecodeFrame avec un fichier <br>
 * \brief dont le nom /datas1/run0004/MyFile_eno1@0_0.bin
 */


/**
 * \file decodeframe.cpp
 * \brief Library to decode Frame DPGA
 * \author Daniel Lambert & Arthur Bongrand
 * \version 1.0
 * \date 21/11/2017
 * 
 * Library to decode Frame from the DPGA
 * 
 */

#include "decodeframe.h"
#include "color.h"
#include <sstream>
#include <string>
#include <stdio.h>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <vector>
#include <string.h>

using namespace std;

const char *Color(bool c)
{
  if (c) return("OK");
  else return("Nok");
}

int main()
{cout << "\n*************** Main **************** \n" << endl;
  
  
  int NombreDeFragmentATester = 0 ; /* 59796;*/ // ATTENTION <1 for analyse all file
  
  //name file
//   DecodeFrame DecFrame("/home/gate/Documents/DAQ/datas/MyFile_eno2@6_0.bin");
  
    DecodeFrame DecFrame("./../../datas/MyFile_eno2@6_0.bin");

  
  //   A    MyFile_eno1@2_0.bin
  // A    MyFile_eno2@6_0.bin
  
  map < int , vector <int> > map_Trigger_TimeStamp_Thor; 
  map < int , int > map_CompteurPaquetThor;
  map < int ,vector<uint16_t*> > map_Channel;
  map < int ,vector<S_HeaderFrame*> > map_Header;
  map < int ,map < string, vector<int> > > map_NecessaryInformation; // NbrSamples NbrChannels
  
  bool Erase_Maps = true;
  
  int PrintTheHeader 	= 0; // 1-> print only "Fragment & Pointer Rawdata & Start Of Frame" 2 -> All
  int PrintChannels 	= 0;
  int PrintCRC_Eof	= 0;
  
  uint16_t *pCh = NULL;
  
  const char * W_FileName = "Write_Fileeno2@6_0.bin";
  FILE * File = fopen(W_FileName,"w");
  
  
  //**** Write one Header Now Hearder of the first card ***/ 
  S_HeaderFile *HeaderFile = DecFrame.GetHeaderFile();
  if(!HeaderFile){cout << "No Header File please check datas ! exit(0)" << endl; exit(0);}
  fwrite(HeaderFile,sizeof(S_HeaderFile),1,File ); 
  //   cout << "HeaderFile " << HeaderFile << endl;
  
  // LimitForTest function
  bool LimitForTest = false;
  if(NombreDeFragmentATester>0)LimitForTest = true;
  if(LimitForTest == false) cout << "WARNING Mode Complete Analyse Data" << endl;
  else cout << "WARNING Analyse only " <<  NombreDeFragmentATester << " fragments" << endl; 
  // For Statistiques
  int TotTriggerThor_EventComplete=0;
  
  
  //DecFrame.Print((uint16_t *) HdrFrame,80);
  unsigned long Index=0;
  int z =1;
  do {
    
    uint16_t *pRawData = DecFrame.NextPacket(); // pointeur sur channel 1 et non sur le header
    S_HeaderFrame *HdrFrame = DecFrame.GetHeaderFrame();  // Recuperer le header de la frame
    
    if(!pRawData || !HdrFrame ) {cout<<"break -> Eof "<<endl;break;} // ATTENTION NECESSARY FOR STOP AT THE END OF THE File
    
    int CptTriggerThor =  static_cast<int>(DecFrame.GetCptTriggerThor());
    int CptTriggerAsm =  static_cast<int>(DecFrame.GetCptTriggerAsm());
    int NbSamples = static_cast<int>(DecFrame.GetNbSamples());
    
    int NbrChannels = 0;
    
    
    //********************************************************************
    //
    // 		Print infos
    //
    //********************************************************************
    
    
    //********************************************************************
    // 		Header
    //********************************************************************
    if(PrintTheHeader >0){
      printf("Fragement %lu **************************************************\n",Index++);
      printf("Pointer Rawdata %p Start Of Frame 0x%x\n",pRawData,HdrFrame->StartOfFrame);
      printf("Nombre trigger Thor\t%d\n",DecFrame.GetCptTriggerThor());
      if(PrintTheHeader >1 ){
	printf("Start of Frame \t\t%s\n",Color(DecFrame.IsSoFOk()));
	printf("Nombre Frame AMC\t%d\n",DecFrame.GetNbFrameAmc());
	printf("Front end Id \t\t0x%x\n",DecFrame.GetFeId());
	printf("Asm Mode \t\t%d\n",DecFrame.GetMode());
	printf("Trigger Type \t\t0x%x\n",DecFrame.GetTiggerType());
	printf("Nombre Frame ASM\t%lu\n",DecFrame.GetNbFrameAsm());
	printf("CafeDeca \t\t%s\n",Color(DecFrame.IsCafeDecaOk()));
	printf("Undefined \t\t0x%lx\n",DecFrame.GetUndefined());
	printf("TimeStamp ASM\t\t%f ms\n",DecFrame.GetTimeStpAsm()*1/**6.66/1000000*/);
	printf("TimeStamp Thor vs ASM\t%3.1f ns\n",DecFrame.GetTimeStpThorAsm()*1/**6.66*/);	
	printf("Thor TT \t\t0x%x\n",DecFrame.GetThorTT());
	printf("Pattern \t\t0x%lx\n",DecFrame.GetPattern());	
	printf("Bobo \t\t\t%s\n",Color(DecFrame.IsBoboOk()));
	printf("TimeStamp Thor\t\t%f ms\n",DecFrame.GetTimeStpThor()*1/**4.16/1000000*/);
	
	printf("Nombre trigger Asm\t%d\n",DecFrame.GetCptTriggerAsm());
	printf("Nombre de Samples \t%d\n",DecFrame.GetNbSamples());
	printf("End of Frame\t\t%s\n",Color(DecFrame.IsEoFOk()));
      }
    }
    
    //********************************************************************
    // 		Channels
    //********************************************************************
    
    
    
    if(PrintChannels >0 ){   cout << "Channel before map " <<endl;}
    for (int i=0;i<4;++i){
      pCh = DecFrame.GetChannel(i);
      if (pCh != NULL) {
	if(PrintChannels >0){ printf("Channel Number = %d\n",DecFrame.GetCh()); }
	NbrChannels++;
	if(PrintChannels >1 ){ DecFrame.Print(pCh,80);} // 80 premiers caractère de channel
	// 	  DecFrame.SaveChannel("./datas1/run0004/Channel_",pCh);
	// 				DecFrame.SaveChannel("/Channel_",pCh);
      }
      else break;
    }
    
    
    //     if(pCh == NULL){cout << " WARNING :: CHANNEL WITH NO DATA "  << endl; exit(0) ; }
    
    //********************************************************************
    // 		CRC + Eof
    //********************************************************************
    if(PrintCRC_Eof >0){
      cout << "CRC " << hex<<DecFrame.GetCRC(pRawData)<<dec << endl;
      cout << "Eof " << hex<<unsigned(DecFrame.GetEof(pRawData))<<dec << endl;
    }
    // Pour l'esthétique
    if( (PrintChannels || PrintTheHeader || PrintCRC_Eof ) >0) printf("\n");
    
    
    
    //********************************************************************
    // 		Map For constitute event
    //********************************************************************
    
    //*** Map For constitute event
    if( pRawData && HdrFrame ){
      
      
      //       fwrite(HdrFrame,sizeof(S_HeaderFrame),1,File ); 
      //       fwrite(pRawData,(((NbSamples+2)*4)+2),2,File);  // ( NbSamples + 2 [comment #channel + SROUT] ) * Nbr Channels + 2 [controle word at the end], 2 [to convert in octect]
      
      
      // WARNING Necessary for use map !!!
      S_HeaderFrame * HdrFrame_Copy = ( S_HeaderFrame *) malloc(sizeof(S_HeaderFrame));
      //       cout << "Adress of Copy pointeur"<< HdrFrame_Copy << endl;
      memcpy ( HdrFrame_Copy, HdrFrame, sizeof(S_HeaderFrame) );
      
      int size_temp = (HeaderFile->NbSamples*2+8); // size of one channel
      
      
      //       cout << "Channels " << NbrChannels << "\n"<< endl;
      
      
      uint16_t * pRawData_Copy = (uint16_t *) calloc(NbrChannels,size_temp);
      
      //       memcpy (pRawData_Copy, pRawData, sizeof(uint16_t));
      
      
      //        *pRawData_Copy = *pRawData;
      memcpy (pRawData_Copy, pRawData,NbrChannels*size_temp);
      
      //       cout << "Adress of original pointeur "<< pRawData << endl;      
      //       cout << "Adress of Copy pointeur "<< pRawData_Copy << endl;
      
      
      //       uint16_t * pRawData_Copy = pRawData;
      
      map_CompteurPaquetThor[CptTriggerThor]+=1;
      map_Channel[CptTriggerThor].push_back(pRawData_Copy);
      map_Header[CptTriggerThor].push_back(HdrFrame_Copy);
      map_NecessaryInformation[CptTriggerThor]["NbSamples"].push_back(NbSamples);
      map_NecessaryInformation[CptTriggerThor]["NbrChannels"].push_back(NbrChannels);
      
      
      
      if(map_CompteurPaquetThor[CptTriggerThor]==6  ){
	
	if(Erase_Maps) TotTriggerThor_EventComplete+=1;
	
	if(CptTriggerThor%10000 == 0)cout << "Write CptTriggerThor # " << CptTriggerThor << endl;
	
	for(int i =0; i< map_Channel[CptTriggerThor].size();i++ ){
	  
	  S_HeaderFrame *HdrFrame_Temp	= map_Header[CptTriggerThor][i];
	  uint16_t *pRawData_Temp	= map_Channel[CptTriggerThor][i];
	  int NbSample_Temp	= map_NecessaryInformation[CptTriggerThor]["NbSamples"][i];
	  int NbrChannels_Temp	= map_NecessaryInformation[CptTriggerThor]["NbrChannels"][i];
	  
	  uint16_t Crc	= DecFrame.GetCRC(pRawData_Temp);
	  uint8_t Eof	= DecFrame.GetEof(pRawData_Temp);
	  
	  // 	  cout << "Eof " <<  hex<<unsigned(Eof)<<dec << "  "<< Color(DecFrame.IsEoFOk(Eof)) << endl;
	  
	  
	  if(Color(DecFrame.IsCrcOk(Crc))=="Nok"){cout << "CRC Data channel after map aren't ok exit(0)" << endl;exit(0);} 
	  if(Color(DecFrame.IsEoFOk(Eof))=="Nok"){cout << "EoF Data channel after map aren't ok exit(0)" << endl;exit(0);} 
	  
	  fwrite(HdrFrame_Temp,sizeof(S_HeaderFrame),1,File ); 
	  // 	  fwrite(pRawData_Temp,(((1024+(2))*4)+2),2,File ); 
	  fwrite(pRawData_Temp,(((NbSample_Temp+(2))*NbrChannels_Temp)+2),2,File ); 
	}
	
	if(Erase_Maps){
	  map_CompteurPaquetThor.erase(CptTriggerThor);
	  map_Channel.erase(CptTriggerThor);
	  map_Header.erase(CptTriggerThor);
	  map_NecessaryInformation.erase(CptTriggerThor);
	}
      }
    }
    
    
    if(z == NombreDeFragmentATester && LimitForTest == true ){cout << "Stop after "<< z <<" trames" <<endl; break;}
    z++;
  }	while ((pCh != NULL)); // pCh != Null -> Mettre une valeur si on ne veux pas décoder une channel
  
  
  
  // 	for(auto& x: map_CompteurPaquetThor){
  // 	  cout <<" Nb Triggger Thor aka Event " << x.first <<" Nbr tot paquet " << x.second <<endl;
  // 	  break;
  // 	}
  
  z=1; // Trigger begin at 1
  
  int LostPacket=0;
  int TotTriggerThor = TotTriggerThor_EventComplete;
  
  map <int , int> map_Lost_Packet_Event;
  
  for(auto& x: map_Channel){
    TotTriggerThor+=1;
    if(map_CompteurPaquetThor[z]<6){ 
      
      if(Erase_Maps==false)cout<<" Nb Triggger Thor aka Event " << x.first << " Nbr tot paquet" << map_CompteurPaquetThor[z] << endl;
      
      int Lost_Packet_Event = (6-map_CompteurPaquetThor[x.first]);
      
      LostPacket += Lost_Packet_Event;
      map_Lost_Packet_Event[Lost_Packet_Event]+=1;
    }
    z++;
  }
  
  double Tot_Lost_Packet_Event_ForMean_Denominateur=0;
  double Tot_Lost_Packet_Event_ForMean_Numerateur=0;
  for(auto& x: map_Lost_Packet_Event){
    Tot_Lost_Packet_Event_ForMean_Numerateur+=(double)x.first*(double)x.second;
    Tot_Lost_Packet_Event_ForMean_Denominateur+=x.second;
  }
  
  double MeanPacketLostPerEvent = (double)Tot_Lost_Packet_Event_ForMean_Numerateur/(double)Tot_Lost_Packet_Event_ForMean_Denominateur;
  double PercentEventWithLostPacket = (double)LostPacket/(double)TotTriggerThor*100;
  
  cout << "\nLostPacket " << LostPacket << " TotPacket_Event " << TotTriggerThor << " whether " << PercentEventWithLostPacket << " %" << endl;
  cout << "if it had some lost Packet : Packet Lost Mean " << MeanPacketLostPerEvent << endl;
  
  cout << "\n*************** End Main ***************\n" << endl;
  
} // end main
