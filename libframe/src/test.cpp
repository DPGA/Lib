/**
 * \brief Programme de test pour la librairie libframe <br>
 * \brief montre comment instantier la classe DecodeFrame avec un fichier <br>
 * \brief dont le nom /datas1/run0004/MyFile_eno1@0_0.bin
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


using namespace std;

const char *Color(bool c)
{
	if (c) return("OK");
	else return("Nok");
}

int main()
{
	 cout << "\n*************** Main **************** \n" << endl;
  
	int NombreDeFragmentATester = 0;
	uint16_t *pCh = NULL;
	//name file
	DecodeFrame DecFrame("/home/gate/Documents/DAQ/ServeurUdp/build/Write_File_Multi.bin");
// 	  DecodeFrame DecFrame("/home/gate/Documents/DAQ/datas/MyFile_eno1@0_0.bin");

	map < int , vector <int> > map_Trigger_TimeStamp_Thor; 
	
	map < uint32_t , int > map_CompteurPaquetThor;
	map < uint32_t ,vector<uint16_t*> > map_ReconstructEvent;
	
	
	int PrintTheHeader = 1;
	int PrintChannels = 1;
	
// 	const char * W_FileName = "Write_File.bin";
// 	FILE * File = fopen(W_FileName,"w");
	
	
	//DecFrame.Print((uint16_t *) HdrFrame,80);
	unsigned long Index=0;
	int z =0;

	do {
		uint16_t *pRawData = DecFrame.NextPacket(); // pointeur sur channel 1
		S_HeaderFrame *HdrFrame = DecFrame.GetHeaderFrame();  // Recuperer le header de la frame
	
		if(!pRawData) break; // ATTENTION NECESSARY IF NOT FILE CONTINUE
		
		int CptTriggerThor =  static_cast<int>(DecFrame.GetCptTriggerThor());
		int CptTriggerAsm =  static_cast<int>(DecFrame.GetCptTriggerAsm());
		
// 		fwrite(HdrFrame,sizeof(S_HeaderFrame),1,File ); 
// 		int NbrChannel =4;
// 		fwrite(pRawData,((DecFrame.GetNbSamples()+(2))*NbrChannel)+2,2,File ); 
		
		if(PrintTheHeader >0){
		
		printf("Fragement %lu **************************************************\n",Index++);
		printf("Pointer Rawdata %p Start Of Frame 0x%x\n",pRawData,HdrFrame->StartOfFrame);
		printf("Start of Frame \t\t%s\n",Color(DecFrame.IsSoFOk()));
		printf("Nombre Frame AMC\t%d\n",DecFrame.GetNbFrameAmc());
		printf("Front end Id \t\t0x%x\n",DecFrame.GetFeId());
		printf("Asm Mode \t\t%d\n",DecFrame.GetMode());
		printf("Trigger Type \t\t0x%x\n",DecFrame.GetTiggerType());
		printf("Nombre Frame ASM\t%lu\n",DecFrame.GetNbFrameAsm());
		printf("CafeDeca \t\t%s\n",Color(DecFrame.IsCafeDecaOk()));
		printf("Undefined \t\t0x%lx\n",DecFrame.GetUndefined());
		printf("TimeStamp ASM\t\t%f ms\n",DecFrame.GetTimeStpAsm());
		printf("TimeStamp Thor vs ASM\t%3.1f ns\n",DecFrame.GetTimeStpThorAsm());	
		printf("Thor TT \t\t0x%x\n",DecFrame.GetThorTT());
		printf("Pattern \t\t0x%lx\n",DecFrame.GetPattern());	
		printf("Bobo \t\t\t%s\n",Color(DecFrame.IsBoboOk()));
		printf("TimeStamp Thor\t\t%f ms\n",DecFrame.GetTimeStpThor());
		printf("Nombre trigger Thor\t%d\n",DecFrame.GetCptTriggerThor());
		printf("Nombre trigger Asm\t%d\n",DecFrame.GetCptTriggerAsm());
		printf("Nombre de Samples \t%d\n",DecFrame.GetNbSamples());
		printf("End of Frame\t\t%s\n",Color(DecFrame.IsEoFOk())); 
		}
		
		
		if(PrintChannels >0){
		for (int i=0;i<4;++i){
			pCh = DecFrame.GetChannel(i);
			if (pCh != NULL) {
				printf("Channel Number = %d\n",DecFrame.GetCh()); 
				DecFrame.Print(pCh,80); // 80 premiers caractère de channel
				DecFrame.SaveChannel("./datas1/run0004/Channel_",pCh);
// 				DecFrame.SaveChannel("/Channel_",pCh);
			}
			else break;
		}
		}
		else{
		 	for (int i=0;i<4;++i){
			pCh = DecFrame.GetChannel(i);
			if (pCh != NULL) {
				break;
			}
			else break;
		}
		if(pCh == NULL) cout << " WARNING :: CHANNEL WITH NO DATA " << endl;
		}
		
		map_CompteurPaquetThor[DecFrame.GetCptTriggerThor()]+=1;
		
		map_ReconstructEvent[DecFrame.GetCptTriggerThor()].push_back(pRawData);
		
		cout << "Eof " << hex<<DecFrame.GetEof(pRawData)<<dec << endl;
		cout << "CRC " << hex<<DecFrame.GetCRC(pRawData)<<dec << endl;
		
		if( (PrintChannels || PrintTheHeader) >0) printf("\n");

	}	while ((pCh != NULL) && (z++ <=NombreDeFragmentATester)); // pCh != Null -> Mettre une valeur si on ne veux pas décoder une channel
	
// 	for(auto& x: map_CompteurPaquetThor){
// 	  cout <<" Nb Triggger Thor aka Event " << x.first <<" Nbr tot paquet " << x.second <<endl;
// 	  break;
// 	}
	
	z=1; // Trigger begin at 1
	
	for(auto& x: map_ReconstructEvent){
	  cout<<" Nb Triggger Thor aka Event " << x.first << " Nbr tot paquet" << map_CompteurPaquetThor[ x.first] << endl;
// 	  if(z >10) break;
	  z++;
	}
	
	
}
