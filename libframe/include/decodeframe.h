/**
 * \file decodeframe.h
 * \brief Header Library to decode Frame DPGA
 * \author Daniel Lambert
 * \version 0.1
 * \date 10/207/2017
 * 
 * Header Library to decode Frame from thne DPGA
 * 
 */


#ifndef __DECODEFRAME_HH__
#define __DECODEFRAME_HH__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "frame.h"
#include <string>
#define VERSION_DECODEFRAME "1.0.0 "

/*!\struct S_ErrorFrame
 * \brief Compte le nombre d'erreur rencontrer dans le decodage d'un fichier
 */
struct S_ErrorFrame {
  /*! \brief Nombre d'erreur start of Frame (0x1230) */
  uint64_t ErrSoF;
  //! \brief Nombre d'erreur CAFEDECA (0xcafedeca)
  uint64_t ErrCafeDeca;
  //!\brief Nombre d'erreur b0b0 (0xb0b0)
  uint64_t	ErrBobo;
  //! \brief Nombre d'erreur Start of Channel (K29_7)
  uint64_t ErrSoc;
  //! \brief Nombre d'erreur Crc (0xb0b0)
  uint64_t ErrCrc;
  //! \brief Nombre d'erreur End Of Frame (K27_7)
  uint64_t ErrEoF;
  //! \brief Erreur TT
  uint64_t ErrTT;
};

std::string getVersionDecodeFrame();

 
/*!\class DecodeFrame decodeframe.h "include/decodeframe.h"
 * \brief classe permettant de decoder les trames à partir d'un fichier
 * \brief ou d'un pointeur sur une zone mémoire
 */


class DecodeFrame {
	
 private 	:
  const char *FileName;
  FILE *File;
  struct S_HeaderFile *HeaderFile;
  struct S_HeaderFrame *HeaderFrame;
		
  // 		struct S_HeaderFrame *HeaderFrame_Copy;
  // 		
  // 		uint16_t *pChannel_Copy;
  // 		
		
  uint16_t *pChannel;
  uint16_t Index;
  uint8_t curChannel;
  uint16_t Soc;
  uint16_t curSrout;
  uint16_t Crc;
  uint8_t EoF;
  S_ErrorFrame ErrFrame;
  bool EraseAlloc;
  std::vector <uint64_t> TstpAsm;
  std::vector <uint64_t> TstpThor;
  std::vector <uint64_t> TstpThorAsm;
  std::vector <uint64_t> Pattern;
  std::vector <bool> TTError;
		
  // Encode
  FILE *File_Write;
		
		
		
 public	:
		
  bool SetFile_Write(const char* filename);
  bool SetPacket_Write(uint16_t *Packet);
		
  /**
   * \fn DecodeFrame (const char* filename)
   * \brief Fonction de création d'une nouvelle instance d'un objet DecodeFrame.
   * \param filename : Nom du fichier à decoder.
   */ 
  DecodeFrame(const char* filename);
  /**
   * \brief Fonction de création d'une nouvelle instance d'un objet DecodeFrame.
   * \brief Alloue de la mémoire pour la structure HeaderFile 
   * \brief et l'initialise à zero <br>
   * \brief Géneralement appelée par le constructeur d'une classe parente
   */ 
  DecodeFrame();
		
		
		
  /**
   * \brief Destructeur de la classe
   * \brief libère la mémoire utilisée
   */
  ~DecodeFrame();
  /**
   * \brief Donne un nom de Fichier à décoder
   * \brief le constructeur DecodeFrame() doit être préalablement appellé
   * \param filename : Nom du fichier à decoder.
   * \return true si le fichier existe sinon false
   */
  bool SetFile(const char* filename);
  /**
   * \brief Permet de récupérer le header du fichier
   *	\brief voir la structure S_HeaderFile
   * \return S_HeaderFile
   */
  S_HeaderFile *GetHeaderFile();
  /**
   * \brief Donne un pointeur sur un fragment à décoder
   * \brief le constructeur DecodeFrame() doit être préalablement appellé
   * \param Packet : pointeur sur le debut du fragment à decoder.
   * \return true (à voir si false existera)
   */	
  bool SetPacket(uint16_t *Packet);
  bool SetPacket(uint16_t *Packet,uint16_t lg);
  /**
   * \return true si le fichier existe sinon false
   */	
  bool FileIsOpen() {return true ? File == NULL : false;};
  /**
   * \brief Permet de recupérer le header du dernier fragment décodé
   * \return struct S_HeaderFrame
   */
  S_HeaderFrame *GetHeaderFrame();
  /**
   * \brief Test quelques octets significatifs dans la trame
   * \return true si pas d'erreur sinon false
   */
  bool FrameError();
  /**
   * \brief Test quelques octets significatifs dans la trame sans les erreurs TT
   * \return true si pas d'erreur sinon false
   */
  bool FrameErrornoTT();
  /**
   * \brief Test le premier octet de la trame (0x1230)
   * \return true si pas d'erreur sinon false
   */
  inline bool IsSoFOk() {return true ? (ntohs(HeaderFrame->StartOfFrame) == SOF) : false;};
  /**
   * \brief Donne la valeur du compteur AMC du dernier fragment analysé
   * \return La valeur du compteur
   */
  inline uint32_t GetNbFrameAmc() {return ((ntohs(HeaderFrame->NbFrameAmcMsb) << 16) | ntohs(HeaderFrame->NbFrameAmcLsb));};
  /**
   * \brief Donne la valeur du FrontEnd Id contenu dans le header du fichier
   * \return Front end Id
   */
  inline uint8_t  GetFeIdFile() {return HeaderFile->FrontEndId;};
  /**
   * \brief Donne la valeur du FrontEnd Id contenu dans le dernier fragment analysé 
   * \return Front end Id
   */		
  inline uint8_t  GetFeId() {return (HeaderFrame->FeIdK30 & 0x7f);};
  /**
   * \brief Donne la valeur du Mode contenu dans le dernier fragment analysé 
   * \return Mode
   */		
  inline uint16_t GetMode() {return ntohs(HeaderFrame->Mode);};
  /**
   * \brief Donne la valeur du Trigger type contenu dans le dernier fragment analysé 
   * \return Trigger Type
   */				
  inline uint16_t GetTiggerType() {return ntohs(HeaderFrame->TriggerType);};
  /**
   * \brief Donne la valeur du compteur ASM du dernier fragment analysé
   * \return Trigger Type
   */	
  inline uint64_t GetNbFrameAsm() {return (((uint64_t) ntohs(HeaderFrame->NbFrameAsmMsb) << 48) | 
					   ((uint64_t) ntohs(HeaderFrame->NbFrameAsmOsb) << 32) | 
					   ((uint64_t)ntohs(HeaderFrame->NbFrameAsmUsb) << 16) |
					   (uint64_t)(ntohs(HeaderFrame->NbFrameAsmLsb)));};
  /**
   * \brief Test quelques octets particuliers de la trame (0xcafedeca)
   * \return true si pas d'erreur sinon false
   */
  inline bool IsCafeDecaOk() {return true ? ((ntohs(HeaderFrame->Cafe) == 0xcafe) & (ntohs(HeaderFrame->Deca) == 0xdeca)) : false;};
  /**
   * \brief Pour l'instant ce nombre est une constante
   * \return 0x123456789abcdef
   */
  uint64_t GetUndefined() {return (((uint64_t) ntohs(HeaderFrame->undefinedMsb) << 48) | 
				   ((uint64_t) ntohs(HeaderFrame->undefinedOsb) << 32) | 
				   ((uint64_t) ntohs(HeaderFrame->undefinedUsb) << 16) |
				   (uint64_t) (ntohs(HeaderFrame->undefinedLsb)));};
  /**
   * \brief Donne la valeur du TimeStamp fourni par la carte ASM du dernier fragment analysé 
   * \brief en nombre de coup d'horloge de 150 Mhz(6.66 ns) depuis le début de la DAQ
   * \return Valeur TimeStamp ASM  
   */
  inline uint64_t GetTimeStpAsm() {return (((uint64_t) ntohs(HeaderFrame->TimeStampAsmMsb) << 48) | 
					   ((uint64_t) ntohs(HeaderFrame->TimeStampAsmOsb) << 32) | 
					   ((uint64_t) ntohs(HeaderFrame->TimeStampAsmUsb) << 16) |
					   (uint64_t) (ntohs(HeaderFrame->TimeStampAsmLsb)));};
  /**
   * \brief Donne la valeur du TimeStamp entre le trigger hard ASM
   * \brief et le retour par lafibre de la carte AMC du dernier fragment analysé 
   * \brief en nombre de coup d'horloge de 150 Mhz(6.66 ns) 
   * \return Valeur TimeStamp entre Thor et ASM  via AMC 
   */
  inline uint64_t GetTimeStpThorAsm() {return (((uint64_t) ntohs(HeaderFrame->TimeStampTrigThorAsmMsb) << 48) | 
					       ((uint64_t) ntohs(HeaderFrame->TimeStampTrigThorAsmOsb) << 32) | 
					       ((uint64_t) ntohs(HeaderFrame->TimeStampTrigThorAsmUsb) << 16) |
					       (uint64_t) (ntohs(HeaderFrame->TimeStampTrigThorAsmLsb)));};
  /**
   * \brief donne l'information si Trigger fibre non reçu
   * \return false si le trigger fibre est bien reçu sinon true
   * \brief si la reponse est true, les infomations TT,Pattern,TimestampThor,Trigger count sont éronnées
   */												
  inline bool IsErrorTT() {return (ntohs(HeaderFrame->ThorTT)>>15);};
  /**
   * \brief Donne la valeur du type de trigger choisi dans Thor du dernier fragment analysé
   * \brief plus d'explication à venir
   * \return Valeur du trigger type 
   */
  inline uint16_t GetThorTT() {return (ntohs(HeaderFrame->ThorTT)&0x7fff);};
  /**
   * \brief Donne la valeur du pattern provoquant le trigger du dernier fragment analysé
   * \brief plus d'explication à venir
   * \return Valeur du pattern
   */		
  inline uint64_t GetPattern () {return (((uint64_t) ntohs(HeaderFrame->PatternMsb) << 32) | 
					 ((uint64_t) ntohs(HeaderFrame->PatternOsb) << 16) |
					 (uint64_t) (ntohs(HeaderFrame->PatternLsb))) & 0xFFFFFFFFF;};
  /**
   * \brief Test 2 octets de la trame (0xb0b0)
   * \return true si pas d'erreur sinon false
   */												
  inline bool IsBoboOk() {return true ? (ntohs(HeaderFrame->Bobo) == 0xb0b0) : false;};
  /**
   * \brief Donne la valeur du TimeStamp fourni par la carte thor du dernier fragment analysé 
   * \brief en nombre de coup d'horloge de 240 Mhz(4.16 ns) depuis le début de la DAQ (à verifier)
   * \return Valeur TimeStamp Thor  
   */		
  inline uint64_t GetTimeStpThor() {return (((uint64_t) ntohs(HeaderFrame->ThorTrigTimeStampMsb) << 32) | 
					    ((uint64_t) ntohs(HeaderFrame->ThorTrigTimeStampOsb) << 16) |
					    (uint64_t) (ntohs(HeaderFrame->ThorTrigTimeStampLsb)));};
  /**
   * \brief 
   * \return Valeur compteur de trigger
   */
  inline uint32_t GetCptTriggerThor() { return ((ntohs(HeaderFrame->CptTriggerThorMsb) << 16) | ntohs(HeaderFrame->CptTriggerThorLsb));};
  /**
   * \brief Donne la valeur du compteur de trigger fourni par la carte ASM du dernier fragment analysé 
   * \return Valeur du compteur ASM  
   */
  inline uint32_t GetCptTriggerAsm() { return ((ntohs(HeaderFrame->CptTriggerAsmMsb) << 16) | ntohs(HeaderFrame->CptTriggerAsmLsb));};
  /**
   * \brief Donne la valeur du nombre d'echantillons du dernier fragment analysé 
   * \return Valeur TimeStamp Thor  
   */
  inline uint16_t GetNbSamples() {return ntohs(HeaderFrame->NbSample);};
  /**
   * \brief Test le premier octet du du fragment de la voie DRS
   * \return true si = k29_7
   */
  inline bool 		IsSoCOk() {printf("soc= %x\n",Soc);return true ? (Soc == K29_7) : false;};
  /**
   * \brief donne le numero de la voie du dernier fragment analysé
   * \return Valeur de la voie entre 0 et 23
   */		
  inline uint8_t 	GetCh() {return curChannel;};
  /**
   * \brief donne la valeur du srout de la voie du dernier fragment analysé
   * \return Valeur du SrOut
   */		
  inline uint16_t 	GetSrout() {return curSrout;};
  /**
   * \brief Calculera le Crc de la trame (actuellement c'est une constante (0x9876)
   * \return true si Crc ok sinon false 
   */		
  inline bool 		IsCrcOk() {return true ? (Crc == 0x9876) : false;};
  inline bool 		IsCrcOk(uint16_t CRC) {return true ? (CRC == 0x9876) : false;};
  /**
   * \brief Test le dernier octet du fragment
   * \return true si K27_7  sinon false 
   */
  inline bool 		IsEoFOk() {return true ? (EoF == K27_7) : false;};
  inline bool 		IsEoFOk(uint8_t EOF_temp){return true ? (EOF_temp == K27_7) : false;};
  //////////
  /**
   * \brief Donne la valeur du CRC contenu à la fin fichier
   * \return Front end Id
   */
	
  inline uint16_t  GetCRC(uint16_t * pRawdata) {
		
    // Optimisation possible mettre le nombre de channels fixé à 4
    int NbrChannel = 4;
    // 		  printf("sizeof(S_HeaderFrame) %d \n",sizeof(S_HeaderFrame));
    // 		  printf("GetNbSamples() %d \n",GetNbSamples());
    // 		  printf("NbrChannel %d \n",NbrChannel);
    // 		  printf("Pointer Rawdata %p \n",pRawdata);
    return ntohs(pRawdata[((GetNbSamples()+(2))*NbrChannel)]);};
		
  ////////
		
		  
  //////////
  /**
   * \brief Donne la valeur du CRC contenu à la fin fichier
   * \return Front end Id
   */
	
  inline uint8_t  GetEof(uint16_t * pRawdata) {
		
    // Optimisation possible mettre le nombre de channels fixé à 4
    int NbrChannel = 4;
    // 		  return ntohs(pRawdata[((GetNbSamples()+(2))*NbrChannel)+1]);
    return ntohs(pRawdata[(HeaderFile->NbSamples+2)*NbrChannel+1]);

		  
  };
  // + 2 correspond au CRC
  ////////
		  
		  
		  
		  
  /**
   * \brief Permet de décoder le fragment suivant lu dans le fichier
   * \return pointeur sur le premier channel du fragment sinon NULL
   */		
  uint16_t *NextPacket();
  /**
   * \brief Permet de pointer sur le fragment du channel
   * \param PacketChannel : pointeur sur le fragment 
   * \param withChannel = true le pointeur retourné sera avec le numero de canal et le srout
   * \return pointeur sur le channel du fragment sinon NULL
   */
  uint16_t *NextChannel(uint16_t *PacketChannel,const bool withChannel=true);
  /**
   * \brief Permet de pointer sur le fragment d'une voie Drs
   * \param ch : doone le canal à recuper en 0 et 3
   * \
   * \return pointeur sur le channel du fragment
   */
  uint16_t	*GetChannel(int ch);
  /**
   * \brief Permet d'extraire les information des headers 
   * \brief de tous les fragments d'un fichier
   * \brief et de les stocker dans des std::vector
   */		
  void ExtractHeader();
  /**
   * \brief permet d'extraire tous les erreurs TT
   * \return std::vector valeur des TT 
   */
  std::vector<bool> GetAllTTError() {return TTError;};
  /**
   * \brief permet d'extraire tous les TimeStamp ASM d'un fichier
   * \return std::vector valeur des timestamp 
   */
  std::vector<uint64_t> GetAllTstpAsm() {return TstpAsm;};
  /**
   * \brief permet d'extraire tous les TimeStamp Thor d'un fichier
   * \return std::vector valeur des timestamp 
   */		
  std::vector<uint64_t> GetAllTstpThor() {return TstpThor;};
  /**
   * \brief permet d'extraire tous les TimeStamp entre Thor et ASM via AMC d'un fichier
   * \return std::vector valeur des timestamp 
   */		
  std::vector<uint64_t> GetAllTstpThorAsm() {return TstpThorAsm;};
  /**
   * \brief permet d'extraire tous les patterns d'un fichier
   * \return std::vector valeur des patterns 
   */		
  std::vector<uint64_t> GetAllPattern() {return Pattern;};
  /**
   * \return renvoie une struct S_ErrorFrame avec des compteur d'erreur
   */		
  S_ErrorFrame GetErrFrame() {return ErrFrame;};
		
  /**
   * \brief Sauvegarde les voies drs dans un fichier distinct 
   * \param name : nom du fichier
   * \param p		: pointeur sur le fragment de la voie Drs
   */		
  bool SaveChannel(const char *name,uint16_t * p);
  /**
   * \brief Pour débogage
   * \brief Affiche des valeurs en haxadecimal
   * \param val 	: Pointeur sur les valeurs à afficher
   * \param n		: Nombre de valeur à afficher
   */
  void 		Print(uint16_t *val,uint16_t n);
};


#endif
