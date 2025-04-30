#include <wiringPi.h>
#include <softPwm.h>
//根据公式：PWMfreq = 1 x 10^6 / (100 x range)  pwm=50HZ  range=10^4/pwmfreq
void pwm_write(int pwm_pin)
{
	pinMode(pwm_pin, OUTPUT);
	softPwmCreate(pwm_pin,0,200);// range设置周期分为200步， 周期20ms
	softPwmWrite(pwm_pin,10);//1ms 45度
	delay(1000);
	softPwmStop(pwm_pin);
}
void pwm_stop(int pwm_pin)
{
	pinMode(pwm_pin, OUTPUT);
	softPwmCreate(pwm_pin,0,200);// range设置周期分为200步， 周期20ms
	softPwmWrite(pwm_pin,5);//0.5ms 0度
	delay(1000);
	softPwmStop(pwm_pin);
}
