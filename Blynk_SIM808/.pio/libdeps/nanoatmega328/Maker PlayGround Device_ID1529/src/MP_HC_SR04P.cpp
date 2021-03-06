
#include "MP_HC_SR04P.h"


MP_HC_SR04P::MP_HC_SR04P(uint8_t echo ,uint8_t trig,const String &tag )
  : trig(trig) , echo(echo),tag(tag)
{
  
}

void MP_HC_SR04P::init()  
{	
	pinMode(this->trig,OUTPUT);
	pinMode(this->echo,INPUT);
	MP_Log::i(tag,"Ready");

}

double MP_HC_SR04P::getDistance() {	

	  long cm=0 ;
	  /* loop for filter an hardware error */
      do{
	      digitalWrite(trig, LOW);
	      delayMicroseconds(2);
	      digitalWrite(trig, HIGH);
	      delayMicroseconds(5);
	      digitalWrite(trig, LOW);
	      cm = pulseIn(echo, HIGH) / 29.0 /2.0 ;
  		}while(cm>3000);
  	   MP_Log::i(tag,cm);
      return cm;

}

