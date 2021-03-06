#ifndef MP_TMP007_H
#define MP_TMP007_H

//#include "MP_Temp.h"
#include <Wire.h>
#include "Adafruit_TMP007.h"
#include <Arduino.h>
#include "MP_Log.h"


class MP_TMP007  //: MP_Temp
{
  public:
      MP_TMP007(const String &tag);
	 ~MP_TMP007() {};

	
	 double getTemperature()  ;
	 void init() ;

  private:
	  Adafruit_TMP007 tmp007;
	  const String tag;
	 

};

#endif