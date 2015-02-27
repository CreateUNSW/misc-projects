/* 
Bluetooth tankbot v12
CREATE UNSW

Written by: William Hales
Last modified: 2015 02 27

Arduino Robotics unofficial chapter 14, re-fashioned by WJH in 15 BC.
*/

#define MAX_SPEED 255
#define LESSER_SPEED 150

int LED = 13;
bool LED_state = 0;
// L298 motor control pins
int ML_F = 12;
int ML_PWM = 11;
int ML_B = 10;
int MR_F = 2;
int MR_PWM = 3;
int MR_B = 4;
// Motor target and actual speeds
int ML_target = 0;
int ML_actual = 0;
int MR_target = 0;
int MR_actual = 0;
int accel = 256; // Speed units per second the actual motor speeds can change
// When direction buttons were pressed, in millisconds
unsigned long B_up_pressed = 0;
unsigned long B_down_pressed = 0;
unsigned long B_left_pressed = 0;
unsigned long B_right_pressed = 0;
unsigned long B_decaytime = 105; // How long buttons are virtually 'held' for

unsigned long last_time = 0; // When the last loop was run

void setup() {
  TCCR2B = TCCR2B & 0b11111000 | 0x01; // change PWM frequency for pins 3 and 11 to 32kHz so there will be no motor whining
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(ML_F, OUTPUT);
  pinMode(ML_PWM, OUTPUT);
  pinMode(ML_B, OUTPUT);
  pinMode(MR_F, OUTPUT);
  pinMode(MR_PWM, OUTPUT);
  pinMode(MR_B, OUTPUT);
  // Avoid us thinking we've already pressed our buttons
  delay( B_decaytime);
}

void loop() {
  unsigned long now = millis();
  unsigned long later = now + B_decaytime;
  while (Serial.available() > 0)
  {
    switch ( Serial.read() )
    {
      case 'i': B_up_pressed = later; B_down_pressed = 0; break;
      case 'j': B_left_pressed = later; B_right_pressed = 0; break;
      case 'k': B_down_pressed = later; B_up_pressed = 0; break;
      case 'l': B_right_pressed = later; B_left_pressed = 0; break;
      default: ;
    }
  }
  // Determine our new speed targets (and hence direction)
  if ( B_up_pressed >= now )
  {
    if (B_left_pressed >= now)
    {
      ML_target = LESSER_SPEED;
      MR_target = MAX_SPEED;
    }
    else if (B_right_pressed >= now)
    {
      ML_target = MAX_SPEED;
      MR_target = LESSER_SPEED;
    }
    else
    {
      MR_target = MAX_SPEED;
      ML_target = MAX_SPEED;
    }
  }
  else if (B_down_pressed >= now)
  {
    if (B_left_pressed >= now)
    {
      ML_target = -LESSER_SPEED;
      MR_target = -MAX_SPEED;
    }
    else if (B_right_pressed >= now)
    {
      ML_target = -MAX_SPEED;
      MR_target = -LESSER_SPEED;
    }
    else
    {
      MR_target = -MAX_SPEED;
      ML_target = -MAX_SPEED;
    }
  }
  else if ( B_left_pressed >= now )
  {
    ML_target = -MAX_SPEED;
    MR_target = MAX_SPEED;
  }
  else if ( B_right_pressed >= now )
  {
    ML_target = MAX_SPEED;
    MR_target = -MAX_SPEED;
  }
  else
  {
    ML_target = 0;
    MR_target = 0;
  }
  // Determine new motor speeeds
  //int velocity_change_this_tick = ((now - last) * accel) / 1000); // might be too small, cause rounding issues
  int velocity_change_this_tick = 5;
  if ( ML_target > ML_actual )
    ML_actual += velocity_change_this_tick;
  else if (ML_target < ML_actual )
    ML_actual -= velocity_change_this_tick;
  if ( MR_target > MR_actual )
    MR_actual += velocity_change_this_tick;
  else if (MR_target < MR_actual )
    MR_actual -= velocity_change_this_tick;
  // Set motor speeds
  if ( ML_actual > 0 )
  {
    digitalWrite( ML_F, 1);
    digitalWrite( ML_B, 0);
    analogWrite( ML_PWM, ML_actual);
  }
  else
  {
    digitalWrite( ML_F, 0);
    digitalWrite( ML_B, 1);
    analogWrite( ML_PWM, 0 - ML_actual );
  }
  if ( MR_actual > 0 )
  {
    digitalWrite( MR_F, 1);
    digitalWrite( MR_B, 0);
    analogWrite( MR_PWM, MR_actual);
  }
  else
  {
    digitalWrite( MR_F, 0);
    digitalWrite( MR_B, 1);
    analogWrite( MR_PWM, 0 - MR_actual );
  }

  //Serial.print( ML_actual );
  //Serial.print( "  ");
  //Serial.print( MR_actual );
  //Serial.print(" yay\n");
  last_time = now;
  delay(5);
}
