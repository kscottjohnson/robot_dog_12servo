#ifndef LegServo_h
#define LegServo_h

class LegServo {
  public:
    LegServo(Adafruit_PWMServoDriver* driver, uint8_t num, uint16_t defaultPWM, 
      uint16_t defaultAngle);
    void setDefault();
    void setAngle(float ang);
  private:
    Adafruit_PWMServoDriver* _driver;
    uint8_t _num;
    uint16_t _defaultPWM;
    uint16_t _defaultAngle;
    void _setPWM(uint16_t pwm);
};

LegServo::LegServo(Adafruit_PWMServoDriver* driver, uint8_t num, uint16_t defaultPWM, 
  uint16_t defaultAngle){
  
  _driver = driver;
  _num = num;
  _defaultPWM = defaultPWM;
  _defaultAngle = defaultAngle;
}

void LegServo::_setPWM(uint16_t pwm){
  this->_driver->setPWM(this->_num, 0, pwm);
}

void LegServo::setDefault(){
  this->_setPWM(this->_defaultPWM);
}

void LegServo::setAngle(float ang){
  uint16_t pwm = (ang - this->_defaultAngle) * 2.7 + this->_defaultPWM;
  this->_driver->setPWM(this->_num, 0, pwm);
}

#endif