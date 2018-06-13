/**
 * \brief Programme qui construit les event pour la librairie libframe <br>
 * \brief montre comment instantier la classe DecodeFrame avec un fichier <br>
 * \brief dont le nom /datas1/run0004/MyFile_eno1@0_0.bin
 */


/**
 * \file BuildEvent_Multi.cpp
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

#include <dirent.h>

#include <cstring>
#include <memory>

#ifndef WIN32
#include <sys/types.h>
#define CLEAR "clear" 
/* system("clear") pour UNIX */
#else
#define CLEAR "cls" 
/* system("cls") pour Windows */
#endif

using namespace std;

const char *Color(bool c){
  if (c) return("OK");
  else return("Nok");
}

int isDir(char* s){ // for search only one character // (char*)(NameOf_fichierLu.c_str())
  if ((strchr(s, '.')) == NULL) /* Si le nom du chemin n'a pas de point (une extension). */
    return 1;
  else
    return 0;
}

int isDir(string str,string extension){
  std::size_t found = str.find(extension);
  if (found!=std::string::npos){
    //   std::cout << "first 'needle' found at: " << found << '\n';
    return 1;
  }
  else return 0;
}

string DoubleToString (double var ){
  string str = std::to_string(var); 
  str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
  //   if(strncmp(&str.back(),".",1)==0)cout <<"str before " <<str << endl;
  if(strncmp(&str.back(),".",1)==0){str.pop_back(); 
    /*cout <<"str after " <<str << endl;*/}
    return str;
}

string IntToString (int var ){
  string str = std::to_string(var); 
  str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
  //   if(strncmp(&str.back(),".",1)==0)cout <<"str before " <<str << endl;
  if(strncmp(&str.back(),".",1)==0){str.pop_back(); 
    /*cout <<"str after " <<str << endl;*/}
    return str;
}

void ReadAndLoadADirectory(vector <string> &v_CompleteNameOfFile,string Path,string NameOfDirectory , string ExtensionOfResearchFile ){
  
  
  DIR* rep = NULL;
  struct dirent* fichierLu = NULL; /* Déclaration d'un pointeur vers la structure dirent. */
  // Mettre le chemin des datas
  
  string EndOfPath ="/";
  string Complete_dir = Path+NameOfDirectory+EndOfPath;
  
  rep = opendir(Complete_dir.c_str());
  if (rep == NULL) exit(1);
  
  while ((fichierLu = readdir(rep)) != NULL){
    string NameOf_fichierLu = fichierLu->d_name;
    if(isDir(NameOf_fichierLu,ExtensionOfResearchFile) == 1)
    { 
      printf("Le fichier lu s'appelle '%s'\n", fichierLu->d_name);
      v_CompleteNameOfFile.push_back(Complete_dir+NameOf_fichierLu);
    }
  }
  cout << "\n";
  
  return;
}

int NumberOfTotalFrameForWriteEvent_Pattern(uint64_t Pattern, int TotalOfAMS) {
  
  int  NumberOfTotalFrameForWriteEvent = -1;
  
//   cout << hex<<Pattern<<dec << " " << TotalOfAMS << endl;
  
  if(Pattern == (uint64_t)0xfffffffff ) return (6* TotalOfAMS);
  else{cout << "Unknow Pattern " << endl; exit(0);} 
  
}

int main()
{cout << "\n*************** Main **************** \n" << endl;
  
  
  //**** Macro Commands
  int NombreDeFragmentATester	=	0 ; /* 59796;*/ // ATTENTION <1 for analyse all file
  
  bool Erase_Maps	= true;
  
  int PrintTheHeader 	= 	0; // 1-> print only "Fragment & Pointer Rawdata & Start Of Frame" 2 -> All
  int PrintChannels 	=	0;
  int PrintCRC_Eof	= 	0;
  
  bool SaveIncompleteEvent	= true; // Bool for Save Uncomplete Event At The end of The file
  
  int StopAfter_X_DataFiles = 1; // Option pour stopper l'analyse après que le premier fichier ait fini (Si les fichiers ont des tailles différentes)
				 // 0 -> Analyse All ||
  
  //******
  
  
  //name file input put directory
  string Path = "./../../";
  string NameOfDirectory = "datas";
  
  
  // Write_File_Multi
  const char * W_FileName = "Write_File_Multi.bin";
  FILE * File = fopen(W_FileName,"w");
  
  // Read and Load the data contained in a directory
  vector <string> v_CompleteNameOfFile;
  string ExtensionOfResearchFile =".bin";
  ReadAndLoadADirectory(v_CompleteNameOfFile, Path, NameOfDirectory,ExtensionOfResearchFile ); 
  
  // Transform Data in a map of DecodeFrame and Write the Header of All File
  map <int, DecodeFrame* > map_LoadAllData;
  S_HeaderFile *HeaderFile;
  
  int indic = 0;
  for(auto& x: v_CompleteNameOfFile){
//     cout << x << endl;
    DecodeFrame *s1 = new DecodeFrame(x.c_str());
    HeaderFile = s1->GetHeaderFile();
    if(!HeaderFile){cout << "No Header File please check datas ! exit(0)" << endl; exit(0);}
//     printf("main Mode file = %d, Feid = %x NbSample= %d\n",HeaderFile->ModeFile,HeaderFile->FrontEndId,HeaderFile->NbSamples);
    
    int NumeroCard = (HeaderFile->FrontEndId);
    int Real_NumeroCard = NumeroCard-16;
    cout <<"#Card dec " << (NumeroCard-16) << " hex " << hex<<NumeroCard<<dec << "\n" <<endl;
    
    if(map_LoadAllData[Real_NumeroCard] != NULL){cout << "DATA of same card exit(0)" << endl; exit(0);}
    else map_LoadAllData[Real_NumeroCard]=s1;
    //**** Write one Header Now Hearder of the first card ***/ 
    if(indic ==0){fwrite(HeaderFile,sizeof(S_HeaderFile),1,File );}
    //******
     indic++;
  }

  // Declare maps
  map < int , vector <int> > map_Trigger_TimeStamp_Thor; 
  map < int , int > map_CompteurPaquetThor;
  map < int ,vector<uint16_t*> > map_Channel;
  map < int ,vector<S_HeaderFrame*> > map_Header;
  map < int ,map < string, vector<int> > > map_NecessaryInformation; // NbrSamples NbrChannels
  
  // For Statistiques
  int TotTriggerThor_EventComplete=0;
  
  // LimitForTest function
  bool LimitForTest = false;
  if(NombreDeFragmentATester>0)LimitForTest = true;
  if(LimitForTest == false) cout << "\nWARNING Mode Complete Analyse Data" << endl;
  else cout << "WARNING Analyse only " <<  NombreDeFragmentATester << " fragments\n" << endl; 
  
  uint16_t *pCh = NULL;
  unsigned long Index=0;
  int z =1;
  
  int StopAfterCheckAllCard = 0;
  int NumberOfTotalFrameForWriteEvent = 6 * map_LoadAllData.size();
  
  
  
  map <int, int > map_For_Break;
  int SizeDataFiles = map_LoadAllData.size();
  
  if(StopAfter_X_DataFiles <=0)StopAfter_X_DataFiles = SizeDataFiles;
  
  do {
    
    if(map_For_Break.size() == StopAfter_X_DataFiles ) break;
    
    for(auto& NumberCard: map_LoadAllData ){
      
      uint16_t *pRawData = NumberCard.second->NextPacket(); // pointeur sur channel 1 et non sur le header
      S_HeaderFrame *HdrFrame = NumberCard.second->GetHeaderFrame();  // Recuperer le header de la frame
      
      if(map_For_Break.size() == StopAfter_X_DataFiles ) break;
      
      if(!pRawData || !HdrFrame ) {
	/*cout<<"break -> Eof "<<endl;*/ 
	map_For_Break[NumberCard.first]+=1;
	delete map_LoadAllData[NumberCard.first] ;
	map_LoadAllData.erase(NumberCard.first) ;
	continue;
	
      } // ATTENTION NECESSARY FOR STOP AT THE END OF THE File
      
      
      int CptTriggerThor =  static_cast<int>( NumberCard.second->GetCptTriggerThor());
      int CptTriggerAsm =  static_cast<int>( NumberCard.second->GetCptTriggerAsm());
      int NbSamples = static_cast<int>( NumberCard.second->GetNbSamples());
      int NbrChannels = 0;
     
     //
     NumberOfTotalFrameForWriteEvent = NumberOfTotalFrameForWriteEvent_Pattern(NumberCard.second->GetPattern(),SizeDataFiles);
     // 
     
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
	printf("Nombre trigger Thor\t%d\n", NumberCard.second->GetCptTriggerThor());
	cout << " ## " << NumberCard.first << endl;
	if(PrintTheHeader >1 ){
	  printf("Start of Frame \t\t%s\n",Color( NumberCard.second->IsSoFOk()));
	  printf("Nombre Frame AMC\t%d\n", NumberCard.second->GetNbFrameAmc());
	  printf("Front end Id \t\t0x%x\n", NumberCard.second->GetFeId());
	  printf("Asm Mode \t\t%d\n", NumberCard.second->GetMode());
	  printf("Trigger Type \t\t0x%x\n", NumberCard.second->GetTiggerType());
	  printf("Nombre Frame ASM\t%lu\n", NumberCard.second->GetNbFrameAsm());
	  printf("CafeDeca \t\t%s\n",Color( NumberCard.second->IsCafeDecaOk()));
	  printf("Undefined \t\t0x%lx\n", NumberCard.second->GetUndefined());
	  printf("TimeStamp ASM\t\t%f ms\n", NumberCard.second->GetTimeStpAsm()*1/**6.66/1000000*/);
	  printf("TimeStamp Thor vs ASM\t%3.1f ns\n", NumberCard.second->GetTimeStpThorAsm()*1/**6.66*/);	
	  printf("Thor TT \t\t0x%x\n", NumberCard.second->GetThorTT());
	  printf("Pattern \t\t0x%lx\n", NumberCard.second->GetPattern());	
	  printf("Bobo \t\t\t%s\n",Color( NumberCard.second->IsBoboOk()));
	  printf("TimeStamp Thor\t\t%f ms\n", NumberCard.second->GetTimeStpThor()*1/**4.16/1000000*/);
	  printf("Nombre trigger Asm\t%d\n", NumberCard.second->GetCptTriggerAsm());
	  printf("Nombre de Samples \t%d\n", NumberCard.second->GetNbSamples());
	  printf("End of Frame\t\t%s\n",Color( NumberCard.second->IsEoFOk()));
	}
      }
      
      //********************************************************************
      // 		Channels
      //********************************************************************
      
      // WARNING Test Channel For the DO While Condition All 4 channel != NULL
      
      if(PrintChannels >0 ){   cout << "Channel before map " <<endl;}
      for (int i=0;i<4;++i){
	pCh = NumberCard.second->GetChannel(i);
	if (pCh != NULL) {
	  if(PrintChannels >0){ printf("Channel Number = %d\n", NumberCard.second->GetCh()); }
	  NbrChannels++;
	  if(PrintChannels >1 ){ NumberCard.second->Print(pCh,80);} // 80 premiers caractère de channel
	  // 	  NumberCard.second->SaveChannel("./datas1/run0004/Channel_",pCh);
	  // 				 NumberCard.second->SaveChannel("/Channel_",pCh);
	}
	else break;
      }
      
      
      //********************************************************************
      // 		CRC + Eof
      //********************************************************************
      if(PrintCRC_Eof >0){
	cout << "CRC " << hex<< NumberCard.second->GetCRC(pRawData)<<dec << endl;
	cout << "Eof " << hex<<unsigned( NumberCard.second->GetEof(pRawData))<<dec << endl;
      }
      // Pour l'esthétique
      if( (PrintChannels || PrintTheHeader || PrintCRC_Eof ) >0) printf("\n");
      
      //********************************************************************
      //
      // 		Work
      //
      //********************************************************************
      
      //********************************************************************
      // 		Map For constitute event
      //********************************************************************
      
      //*** Map For constitute event
      if( pRawData && HdrFrame ){
	
	// WARNING Necessary for use map !!!
	S_HeaderFrame * HdrFrame_Copy = ( S_HeaderFrame *) malloc(sizeof(S_HeaderFrame)); 
	memcpy ( HdrFrame_Copy, HdrFrame, sizeof(S_HeaderFrame) );
	
	int size_temp = (HeaderFile->NbSamples*2+8); // size of one channel
	uint16_t * pRawData_Copy = (uint16_t *) calloc(NbrChannels,size_temp);
	memcpy (pRawData_Copy, pRawData,NbrChannels*size_temp);
	
	//       cout << "Adress of original pointeur "<< pRawData << endl;      
	//       cout << "Adress of Copy pointeur "<< pRawData_Copy << endl;
	
	map_CompteurPaquetThor[CptTriggerThor]+=1;
	map_Channel[CptTriggerThor].push_back(pRawData_Copy);
	map_Header[CptTriggerThor].push_back(HdrFrame_Copy);
	map_NecessaryInformation[CptTriggerThor]["NbSamples"].push_back(NbSamples);
	map_NecessaryInformation[CptTriggerThor]["NbrChannels"].push_back(NbrChannels);
	
	if(map_CompteurPaquetThor[CptTriggerThor]==NumberOfTotalFrameForWriteEvent  ){
	  
	  if(Erase_Maps) TotTriggerThor_EventComplete+=1;
	  
	  if(CptTriggerThor%1000 == 0)cout << "Write CptTriggerThor # " << CptTriggerThor  << endl;
	  
	  for(int i =0; i< map_Channel[CptTriggerThor].size();i++ ){
	    
	    S_HeaderFrame *HdrFrame_Temp	= map_Header[CptTriggerThor][i];
	    uint16_t *pRawData_Temp	= map_Channel[CptTriggerThor][i];
	    int NbSample_Temp	= map_NecessaryInformation[CptTriggerThor]["NbSamples"][i];
	    int NbrChannels_Temp	= map_NecessaryInformation[CptTriggerThor]["NbrChannels"][i];
	    
	    uint16_t Crc	= NumberCard.second->GetCRC(pRawData_Temp);
	    uint8_t Eof	= NumberCard.second->GetEof(pRawData_Temp);
	    
	    // 	  cout << "Eof " <<  hex<<unsigned(Eof)<<dec << "  "<< Color( NumberCard.second->IsEoFOk(Eof)) << endl;
	    
	    // WARNING Verify CRC AND EOF
	    if(Color( NumberCard.second->IsCrcOk(Crc))=="Nok"){cout << "CRC Data channel after map aren't ok exit(0)" << endl;exit(0);} 
	    if(Color( NumberCard.second->IsEoFOk(Eof))=="Nok"){cout << "EoF Data channel after map aren't ok exit(0)" << endl;exit(0);} 
	    
	    fwrite(HdrFrame_Temp,sizeof(S_HeaderFrame),1,File ); 
	    // 	  fwrite(pRawData_Temp,(((1024+(2))*4)+2),2,File ); 
	    fwrite(pRawData_Temp,(((NbSample_Temp+(2))*NbrChannels_Temp)+2),2,File ); 
	  }
	  
	  if(Erase_Maps){
	    map_CompteurPaquetThor.erase(CptTriggerThor);
	    map_NecessaryInformation.erase(CptTriggerThor);
	    
	    for (vector<uint16_t*>::iterator it = map_Channel[CptTriggerThor].begin() ; it != map_Channel[CptTriggerThor].end(); ++it){delete (*it);} 
	    map_Channel[CptTriggerThor].clear();
	    for (vector<S_HeaderFrame*>::iterator it = map_Header[CptTriggerThor].begin() ; it != map_Header[CptTriggerThor].end(); ++it){delete (*it);} 
	    map_Header[CptTriggerThor].clear();
	    map_Channel.erase(CptTriggerThor);
	    map_Header.erase(CptTriggerThor);
	  }
	}
      }
      if(z == NombreDeFragmentATester && LimitForTest == true ){cout << "Stop after "<< z <<" trames" <<endl; break;}
      z++;
    }
    if(z == NombreDeFragmentATester && LimitForTest == true ){cout << "Stop after "<< z <<" trames" <<endl; break;}
  }	while ((pCh != NULL)); // pCh != Null -> Mettre une valeur si on ne veux pas décoder une channel
  
  // For Test Break of different File
  cout << "\n";
  for(auto& NumberCard: map_LoadAllData ){if(map_For_Break[NumberCard.first] ==0)map_For_Break[NumberCard.first] =0;}
  for(auto& x: map_For_Break){
    cout <<"Card # " << x.first <<" Data end " << x.second <<endl;
  }
  cout << "\nNbr of ASM : " << map_For_Break.size() << endl;
  
  int Event_With_A_Lost_Packet = 0;
  int LostPacket=0;
  int TotTriggerThor = TotTriggerThor_EventComplete;
  map <int , int> map_Lost_Packet_Event;
  
  for(auto& x: map_Channel){
    TotTriggerThor+=1;
    if(map_CompteurPaquetThor[x.first]<NumberOfTotalFrameForWriteEvent){ 
      
      if(Erase_Maps==false)cout<<" Nb Triggger Thor aka Event " << x.first << " Nbr tot paquet" << map_CompteurPaquetThor[x.first] << endl;
      
      int Lost_Packet_Event = (NumberOfTotalFrameForWriteEvent-map_CompteurPaquetThor[x.first]);
      Event_With_A_Lost_Packet+=1;
      LostPacket += Lost_Packet_Event;
      map_Lost_Packet_Event[Lost_Packet_Event]+=1;
    }
  }
  
  double Tot_Lost_Packet_Event_ForMean_Denominateur=0;
  double Tot_Lost_Packet_Event_ForMean_Numerateur=0;
  for(auto& x: map_Lost_Packet_Event){
    Tot_Lost_Packet_Event_ForMean_Numerateur+=(double)x.first*(double)x.second;
    if(x.second>0) Tot_Lost_Packet_Event_ForMean_Denominateur+=x.second;
  }
  
  if(Tot_Lost_Packet_Event_ForMean_Denominateur ==0) Tot_Lost_Packet_Event_ForMean_Denominateur =1;
  double MeanPacketLostPerEvent = (double)Tot_Lost_Packet_Event_ForMean_Numerateur/(double)Tot_Lost_Packet_Event_ForMean_Denominateur;
  double PercentEventWithLostPacket = (double)Event_With_A_Lost_Packet/(double)TotTriggerThor*100;
  
  cout << "\nEvent_With_A_Lost_Packet " << Event_With_A_Lost_Packet << " TotPacket_Event " << TotTriggerThor << " whether " << PercentEventWithLostPacket << " %" << endl;
  cout << "if it had some lost Packet : Packet Lost Mean " << MeanPacketLostPerEvent << endl;
  
  
    if(SaveIncompleteEvent == true){
    
      cout << "\nSave Incomplete Event at the end of file \n" << endl;
      
    	for(auto& CptTriggerThor_it: map_Channel){
	  
	  int CptTriggerThor = CptTriggerThor_it.first;
	  
	  for(int i =0; i< CptTriggerThor_it.second.size();i++ ){
	    
	    S_HeaderFrame *HdrFrame_Temp	= map_Header[CptTriggerThor][i];
	    uint16_t *pRawData_Temp	= map_Channel[CptTriggerThor][i];
	    int NbSample_Temp	= map_NecessaryInformation[CptTriggerThor]["NbSamples"][i];
	    int NbrChannels_Temp	= map_NecessaryInformation[CptTriggerThor]["NbrChannels"][i];
	    
	    fwrite(HdrFrame_Temp,sizeof(S_HeaderFrame),1,File ); 
	    // 	  fwrite(pRawData_Temp,(((1024+(2))*4)+2),2,File ); 
	    fwrite(pRawData_Temp,(((NbSample_Temp+(2))*NbrChannels_Temp)+2),2,File ); 
	  }
	  
	  if(Erase_Maps){
	    map_CompteurPaquetThor.erase(CptTriggerThor);
	    map_NecessaryInformation.erase(CptTriggerThor);
	    
	    for (vector<uint16_t*>::iterator it = map_Channel[CptTriggerThor].begin() ; it != map_Channel[CptTriggerThor].end(); ++it){delete (*it);} 
	    map_Channel[CptTriggerThor].clear();
	    for (vector<S_HeaderFrame*>::iterator it = map_Header[CptTriggerThor].begin() ; it != map_Header[CptTriggerThor].end(); ++it){delete (*it);} 
	    map_Header[CptTriggerThor].clear();
	    map_Channel.erase(CptTriggerThor);
	    map_Header.erase(CptTriggerThor);
	  }
	}
  }
  
  cout << "\n*************** End Main ***************\n" << endl;
  
} // end main
