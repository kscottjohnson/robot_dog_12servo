// Leg calibration, movement and walking algorithms

void centerServos() {
  for(int l=0; l<4; l++){
    legs[l].setDefault();
  } 
}

void stand(){
  /*
  legs[FRONT_LEFT ].setAngles(90, 135, 90);
  legs[FRONT_RIGHT].setAngles(90, 135, 90);
  legs[BACK_LEFT  ].setAngles(90, 135, 90);
  legs[BACK_RIGHT ].setAngles(90, 135, 90);
  */
  
  legs[FRONT_LEFT ].move(0, 0, 93);
  legs[FRONT_RIGHT].move(0, 0, 93);
  legs[BACK_LEFT  ].move(0, 0, 93);
  legs[BACK_RIGHT ].move(0, 0, 93);

}

void balanceTest() {
  legs[FRONT_LEFT ].move( 0, 0, 85);
  legs[FRONT_RIGHT].move( 0, 0, 93);
  legs[BACK_LEFT  ].move( 0, 0, 93);
  legs[BACK_RIGHT ].move( 0, 0, 85);
}

void manualLeg(){
  /*
  legs[0].hip->setAngle(90);
  legs[0].shoulder->setAngle(160);
  legs[0].knee->setAngle(90);
 */
  legs[0].hip->_setPWM(302);
  legs[0].shoulder->_setPWM(300);
  legs[0].knee->_setPWM(300);

}
void demo() {

  //map left x axis to forward/backward
  //float x = 0;
  //float y = map(lx, 0, 255, -30, 30);
  
  //map left x axis to left/right
  float y = 0;
  float x = map(lx, 0, 255, -20, 20);
  
  //map left y axis to hight
  float z = map(ly, 0, 255, 73, 113);

  // map right x axis to roll
  float r = map(rx, 0, 255, -20, 20);
  // map right u axis to pitch
  float p = map(ry, 0, 255, -30, 30);

  legs[FRONT_LEFT ].move( x, y, z-p+r);
  legs[FRONT_RIGHT].move( x, y, z-p-r);
  legs[BACK_LEFT  ].move( x, y, z+p+r);
  legs[BACK_RIGHT ].move( x, y, z+p-r);
}


uint8_t sTick = 0;
uint8_t sTicksPerState = 8;
uint8_t sWalkState = 0;
float sCurrentY[4] = {0,0,0,0}, sCurrentZ[4] = {93,93,93,93};
float    sPrevY[4] = {0,0,0,0},    sPrevZ[4] = {93,93,93,93};
bool sFirstStep = true;
int8_t sDirection = 1; // 1 forward, -1 reverse
uint8_t sLegSpeed = 3; // multiplier for leg movement
uint8_t sWalkStyle = 0; // 0 - walk, 1 - trot
int8_t sWalkSpeed = 0;
int8_t sMaxWalkSpeed = 8;


const uint8_t sLegStateOffset[2][4] = { 
  {0,2,1,3}, // walk
  {0,2,2,0}  // trot
};
const float sWalkY[32] = { // pattern repeats to make the offsets loop
  -7, 0, 7, 6, 5, 4, 3, 2, 1, 0,-1,-2,-3,-4,-5,-6,
  -7, 0, 7, 6, 5, 4, 3, 2, 1, 0,-1,-2,-3,-4,-5,-6
};
const float sWalkZ[32] = {
  93,60,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
  93,60,93,93,93,93,93,93,93,93,93,93,93,93,93,93
};


/*
//define all legs individually and raise leg opposite of leg moving forward
const float sWalkY[4][16] = {
  {-7, 0, 7, 6, 5, 4, 3, 2, 1, 0,-1,-2,-3,-4,-5,-6},
  { 1, 0,-1,-2,-3,-4,-5,-6,-7, 0, 7, 6, 5, 4, 3, 2},
  { 5, 4, 3, 2, 1, 0,-1,-2,-3,-4,-5,-6,-7, 0, 7, 6},
  {-3,-4,-5,-6,-7, 0, 7, 6, 5, 4, 3, 2, 1, 0,-1,-2}
};
const float sWalkZ[4][16] = {
  {93,93,93,93,93,90,93,93,93,93,93,93,93,93,93,93},
  {93,93,93,93,93,93,93,93,93,93,93,93,93,90,93,93},
  {93,93,93,93,93,93,93,93,93,90,93,93,93,93,93,93},
  {93,90,93,93,93,93,93,93,93,93,93,93,93,93,93,93}
};
*/

void staticWalk() { // defines individual static movement for each leg
  
  if(sTick == 0){ // get walk speed at start of walk state
    sWalkSpeed = map(ly, 0, 255, -1 * sMaxWalkSpeed, sMaxWalkSpeed);

    if(sWalkSpeed != 0){
      sTicksPerState =  (float) sMaxWalkSpeed / abs(sWalkSpeed) * 4;
      if(sWalkSpeed > 0){
        sDirection = 1;
      }
      else{ 
        sDirection = -1;
      }
      if(abs(sWalkSpeed) > 4){ // trot if speed >= 5
        sWalkStyle = 1;
      }
      else{ 
        sWalkStyle = 0;
      }
    }
    else {
      sTicksPerState = 16; // for stationary spin
    }

  }
  
  if(sWalkSpeed == 0){

    for(uint8_t l=0; l<4; l++){
      sCurrentY[l] = 0;
      sCurrentZ[l] = 93;
      sPrevY[l] = 0;
      sPrevZ[l] = 93;

      legs[l].move(0, sCurrentY[l], sCurrentZ[l]);
    }
    sTick = 0;
    sWalkState = 0;
    sDirection = 1;
    sFirstStep = true;
  }
  else{
    //moving
    for(uint8_t l=0; l<4; l++){
      uint8_t legState = sWalkState + 4 * sLegStateOffset[sWalkStyle][l];
      if(sTick + 1 == sTicksPerState){ // if this is the last tick just set the targets
        sCurrentY[l] = sWalkY[legState]*sLegSpeed;
        sCurrentZ[l] = sWalkZ[legState];
        //sCurrentY[l] = sWalkY[l][sWalkState]*sLegSpeed;
        //sCurrentZ[l] = sWalkZ[l][sWalkState];
        sPrevY[l] = sCurrentY[l];
        sPrevZ[l] = sCurrentZ[l];
      }
      else{
        sCurrentY[l] += (sWalkY[legState]*sLegSpeed - sPrevY[l]) / sTicksPerState;
        sCurrentZ[l] += (sWalkZ[legState] - sPrevZ[l]) / sTicksPerState;
        //sCurrentY[l] += (sWalkY[l][sWalkState]*sLegSpeed - sPrevY[l]) / sTicksPerState;
        //sCurrentZ[l] += (sWalkZ[l][sWalkState] - sPrevZ[l]) / sTicksPerState;
      }
      if(sFirstStep && legState < 9){
      //if(sFirstStep && sCurrentY[l] > 0 && l != 0){
        // hack to make non-lifting leg not go forward when starting
        sCurrentY[l] = 0; 
        sPrevY[l] = 0;
      }
      legs[l].move(0, sCurrentY[l], sCurrentZ[l]);
    }

    if(++sTick == sTicksPerState){ // new state
      sTick = 0;
      sWalkState += sDirection;
      sFirstStep = false;
      if(sWalkState == 16){ // new cycle
        sWalkState = 0;
      }
      else if(sWalkState == 255){ // new cycle
        sWalkState = 15;
      }
    }
  }  

}