#include <Arduino.h>

extern void sendStatus();
extern strDateTime DateTime;

class dimmer
{
//    byte state=0;
    String name;
    long offTime;
    long onTime;    
public:
    dimmer(String Name);
    byte tarBrightness;
    byte state;
    double dbrightness;
    double dfaderate;
    byte fade;
    byte zcPin;
    void setState(byte State); 
    void toggle();
    byte getState() {return state;};
    String getName() {return name;};
    long getoffTime() {return offTime;};
    long getonTime() {return onTime;};
    void setoffTime(long off) {offTime = off;};
    void setonTime(long on) {onTime = on;};
    void tick();
};

dimmer::dimmer(String Name)
{
  name = Name;
  offTime = 0;
  onTime = 0;
  state = 1;
  tarBrightness = 255;
  dbrightness = 0.0;
  dfaderate = 1.0;
  fade = 0;
  zcPin = 13;
}

void dimmer::setState(byte State)
{
  byte oldstate = state;
  state = State;
  if(state==1)
  {
// TODO figure out how to turn the dimmer on    
//    digitalWrite(pin,HIGH);
  }
  else
  {
//      digitalWrite(pin,LOW);
  }
  if(state != oldstate)
    sendStatus();
}

void dimmer::toggle()
{
  if(state==1)
    setState(0);
  else
    setState(1);
}

void dimmer::tick()
{
  if(offTime > 0)
  {
    offTime--;
    if(offTime==0)
      setState(0);
  }
  if(onTime > 0)
  {
    onTime--;
    if(onTime==0)
      setState(1);
  }
}
