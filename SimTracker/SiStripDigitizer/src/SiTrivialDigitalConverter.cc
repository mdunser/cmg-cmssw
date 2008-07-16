#include "SimTracker/SiStripDigitizer/interface/SiTrivialDigitalConverter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

SiTrivialDigitalConverter::SiTrivialDigitalConverter(float in){
  electronperADC = in;
}

SiDigitalConverter::DigitalVecType
SiTrivialDigitalConverter::convert(const std::vector<double>& analogSignal, edm::ESHandle<SiStripGain> & gainHandle, unsigned int detid){
  
  SiDigitalConverter::DigitalVecType _temp;
  _temp.reserve(analogSignal.size());
  
  SiStripApvGain::Range detGainRange; 
  if(gainHandle.isValid()) detGainRange = gainHandle->getRange(detid);

  
  for ( unsigned int i=0; i<analogSignal.size(); i++) {
    if (analogSignal[i]<=0) continue;
    float gainFactor  = (gainHandle.isValid()) ? gainHandle->getStripGain(i, detGainRange) : 1;
    
    // convert analog amplitude to digital
    int adc = convert( gainFactor*(analogSignal[i]) );
    
    if ( adc > 0) _temp.push_back(SiStripDigi(i, adc));
  }
  return _temp;
}

SiDigitalConverter::DigitalRawVecType
SiTrivialDigitalConverter::convertRaw(const std::vector<double>& analogSignal, edm::ESHandle<SiStripGain> & gainHandle, unsigned int detid){
  
  SiDigitalConverter::DigitalRawVecType _temp;
  _temp.reserve(analogSignal.size());
  
  SiStripApvGain::Range detGainRange; 
  if(gainHandle.isValid()) detGainRange = gainHandle->getRange(detid);
  
  
  for ( unsigned int i=0; i<analogSignal.size(); i++) {
    // Raw Digis: we need all the channels
    //            saturation below the baseline
    //            (baseline=0 if we do not add pedestals)
    //        --> the following line not needed:
    //    if (analogSignal[i]<=0) continue;
    // and replace with this:
    if (analogSignal[i]<=0) {
      _temp.push_back(SiStripRawDigi(0));
      continue;
    }
    
    float gainFactor  = (gainHandle.isValid()) ? gainHandle->getStripGain(i, detGainRange) : 1;
    
    // convert analog amplitude to digital
    int adc = convertRaw( gainFactor*(analogSignal[i]));
    
    _temp.push_back(SiStripRawDigi(adc));
  }
  return _temp;
}


int SiTrivialDigitalConverter::truncate(float in_adc) {
  
  //Rounding teh ADC number instaed of truncating it
  int adc = int(in_adc+0.5);
  /*
    254 ADC: 254<=raw charge < 511
    255 ADC: 512<= raw charge < 1023
  */
  if (adc > 511 ) return 255;
  if (adc > 253) return 254;
  //Protection
  if (adc < 0) return 0;
  return adc;
}


int SiTrivialDigitalConverter::truncateRaw(float in_adc) {
  
  //Rounding the ADC number
  int adc = int(in_adc+0.5);

  if (adc > 1023 ) return 1023;
  //Protection
  if (adc < 0) return 0;
  return adc;
}
