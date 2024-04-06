#ifndef Pid_h
#define Pid_h

class Pid {
  // assumes constant call interval
  public:
    Pid(float* in, float* set, float p, float i, float d);
    float compute();
  private:
    float *_in, *_set;
    float _p, _i, _d;
    float _prevIn, _cumError;
};

Pid::Pid(float* in, float* set, float p, float i, float d) {
  _in = in;
  _set = set;
  _p = p;
  _i = i;
  _d = d;
}

float Pid::compute() {
  float error =  *_set - *_in;
  float dIn = *_in - _prevIn;

  // proportional
  float output = _p * error; 

  // integral
  _cumError += _i * error;
  output += _cumError;

  // derivative
  output += _d * (*_in - _prevIn);

  _prevIn = *_in;

  return output;
}

#endif