/*
 * myServo.h
 *
 * Created: 08.09.2019 07:55:15
 *  Author: Michael
 */ 


#ifndef MYSERVO_H_
#define MYSERVO_H_

void PWM16_init();
void Servo_setAngle(uint32_t angle);
void Servo_set10Bit(uint16_t analogRead);
void Servo_setMs(float dutyCycleInMs);
uint32_t Servo_map10BitToAngle(uint16_t analogRead);

#endif /* MYSERVO_H_ */