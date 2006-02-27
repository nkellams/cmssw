#include <IORawData/CSCTFCommissioning/interface/CSCTFFileReader.h>
#include <errno.h>
#include <string>

#include <DataFormats/FEDRawData/interface/FEDHeader.h>
#include <DataFormats/FEDRawData/interface/FEDTrailer.h>
#include <DataFormats/FEDRawData/interface/FEDNumbering.h>

#include <FWCore/EDProduct/interface/EventID.h>
#include <FWCore/EDProduct/interface/Timestamp.h>
#include <DataFormats/FEDRawData/interface/FEDRawData.h>
#include <DataFormats/FEDRawData/interface/FEDRawDataCollection.h>

#include <FWCore/ParameterSet/interface/ParameterSet.h>
#include <IORawData/CSCTFCommissioning/interface/FileReaderSP.h>

#include <string>
#include <iosfwd>
#include <iostream>
#include <algorithm>
   
using namespace std;
using namespace edm;

FileReaderSP ___ddu;

CSCTFFileReader::CSCTFFileReader(const edm::ParameterSet& pset):DaqBaseReader(){
	// Following code is stolen from IORawData/DTCommissioning
	const std::string & filename = pset.getParameter<std::string>("fileName");
	std::cout << "Opening File: " << filename << std::endl;
	___ddu.openFile(filename.c_str());
}

bool CSCTFFileReader::fillRawData(edm::EventID& eID, edm::Timestamp& tstamp, FEDRawDataCollection& data){
	// Event buffer and its length
  
  size_t length=0;
	
  // Read DDU record
  ___ddu.readNextEvent();
  const unsigned short* dduBuf = reinterpret_cast<unsigned short*>(___ddu.data());
  length = ___ddu.dataLength();
  
  if(!length) return false;
  
  int runNumber   = 0; // Unknown at the level of EMu local DAQ
  int eventNumber =((dduBuf[2])&0x0FFF) | ((dduBuf[3]&0x0FFF)<<12); // L1A Number
  eID = EventID(runNumber,eventNumber);
  

  unsigned short dccBuf[200000+4*4];//, *dccHeader=dccBuf, *dccTrailer=dccBuf+4*2+(length/2);
  memcpy(dccBuf,dduBuf,length);

  // The FED ID
  FEDRawData& fedRawData = data.FEDData( FEDNumbering::getCSCFEDIds().first );
  fedRawData.resize(length);

  copy(reinterpret_cast<unsigned char*>(dccBuf),
       reinterpret_cast<unsigned char*>(dccBuf)+length, fedRawData.data());

    return true;
}
