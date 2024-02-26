// Leg calibration, movement and walking algorithms

void centerServos() {
  for(int l=0; l<4; l++){
    legs[l].setDefault();
  } 
}

void stand(){
  
  legs[FRONT_LEFT ].move(0, 0, 170);
  legs[FRONT_RIGHT].move(0, 0, 170);
  legs[BACK_LEFT  ].move(0, 0, 170);
  legs[BACK_RIGHT ].move(0, 0, 170);

  /*
  legs[FRONT_LEFT ].setAngles(90, 135, 90);
  legs[FRONT_RIGHT].setAngles(90, 135, 90);
  legs[BACK_LEFT  ].setAngles(90, 135, 90);
  legs[BACK_RIGHT ].setAngles(90, 135, 90);
  */
}