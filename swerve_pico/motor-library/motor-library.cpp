#include "quadrature_encoder.cpp"
#include "PWMControl.cpp"

uint EncoderFactory::encoder_count = 0;

/**
 * file contains syntactic sugar to make getting/setting motors easier
 * created as a lazy way to not have to combine all 3 already working files :/
 */

/**
 * Motor class that holds PWMControl and quadrature_encoder class, encapsulates functions to set and read values of 1 motor
 */
class Motor{
    public:
        Motor(int motorPin, int pwmPin, uint encoderPin, float ratio = 1.0, int chan = PWM_CHAN_A, int countMax = COUNT_MAX){
            this->pwmControl = new PWMControl(motorPin, pwmPin, chan, countMax);
            this->encoder = EncoderFactory::createEncoder(encoderPin, ratio);
        }  

        void Setup(){
            this->pwmControl.Setup();
        }
        void SetPwmPercentage(float percentage){
            this->pwmControl.Drive(percentage);
        }

        float GetPosition(){
            return this->encoder.get_pos();
        }

        
        float GetVelocity(){
            return this->encoder.get_velocity();
        }

    private:
    PWMControl pwmControl;
    Encoder encoder;
};