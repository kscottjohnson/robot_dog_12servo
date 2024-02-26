#ifndef LegServo_h
#define LegServo_h

class LegServo {
  public:
    LegServo(Adafruit_PWMServoDriver* driver, uint8_t num, uint16_t defaultPWM, 
      uint16_t defaultAngle, bool isReversed);
    void setDefault();
    void setAngle(float ang);
    bool isReversed();
  private:
    Adafruit_PWMServoDriver* _driver;
    uint8_t _num;
    uint16_t _defaultPWM;
    uint16_t _defaultAngle;
    bool _isReversed;
    void _setPWM(uint16_t pwm);
};

LegServo::LegServo(Adafruit_PWMServoDriver* driver, uint8_t num, uint16_t defaultPWM, 
  uint16_t defaultAngle, bool isReversed){
  
  _driver = driver;
  _num = num;
  _defaultPWM = defaultPWM;
  _defaultAngle = defaultAngle;
  _isReversed = isReversed;
}

void LegServo::_setPWM(uint16_t pwm){
  this->_driver->setPWM(this->_num, 0, pwm);
}

void LegServo::setDefault(){
  this->_setPWM(this->_defaultPWM);
}

void LegServo::setAngle(float ang){
  uint16_t pwm;
  if(this->isReversed()){
    pwm = this->_defaultPWM - ((ang - this->_defaultAngle) * 2.7);
  }
  else{
    pwm = this->_defaultPWM + ((ang - this->_defaultAngle) * 2.7);
  }
  this->_driver->setPWM(this->_num, 0, pwm);
}

bool LegServo::isReversed(){
  return this->_isReversed;
}

class Leg {
  public:
    Leg(LegServo* hipServo, LegServo* shoulderServo, LegServo* kneeServo);
    LegServo* hip;
    LegServo* shoulder;
    LegServo* knee;
    void setDefault();
    void setAngles(float h, float s, float k);
    void move(float x, float y, float z);
};

Leg::Leg(LegServo* hipServo, LegServo* shoulderServo, LegServo* kneeServo){
  hip = hipServo;
  shoulder = shoulderServo;
  knee = kneeServo;
}

void Leg::setDefault(){
  this->hip->setDefault();
  this->shoulder->setDefault();
  this->knee->setDefault();
}

float adjacentAngle(float a) {
  // gets the adjacent angle in a specified quadrilateral with sides 36,36,36,30mm

  return (
    (
      acos(sqrt(2196.0-2160.0*cos(a*DEG_TO_RAD))/72.0) +
      acos((-1800.0+2160.0*cos(a*DEG_TO_RAD))/(-60.0*sqrt(2196.0-2160.0*cos(a*DEG_TO_RAD))))
    )*RAD_TO_DEG
  );
}

void Leg::setAngles(float h, float s, float k){

  this->hip->setAngle(h); 
  this->shoulder->setAngle(s);
  this->knee->setAngle(adjacentAngle(-1 * (k + s - 315)) - 45);
}

void Leg::move(float x, float y, float z){

  float xzLegLen = z, hipAngle = 90; // defaults if x is zero
  if(x != 0){ 
    // get hypotenuse of x and z
    float xzHyp = sqrt(x*x + z*z);

    // get leg len using offset
    xzLegLen = sqrt(xzHyp*xzHyp - LEG_OFFSET*LEG_OFFSET);

    // hip angle derived from x and leg len
    hipAngle = asin(x / xzLegLen) * RAD_TO_DEG + 90;
  }

  // shoulder angle due to y
  float s1 = 0;
  if(y != 0){
    s1 = asin(y / xzLegLen) * RAD_TO_DEG;
  }
  
  // remaining shoulder angle due to knee bend
  float s2 = acos(xzLegLen / 2 / FEMUR) * RAD_TO_DEG;

  float shoulderAngle = s1 + s2 + 90;

  // determine knee angle using s2
  float kneeAngle = 180 - 2 * s2;

  this->setAngles(hipAngle, shoulderAngle, kneeAngle);
}




#endif




