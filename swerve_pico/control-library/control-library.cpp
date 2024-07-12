
#include "pid.cpp"

/** 
 * Class to hold two pids, represents one swerve module (one turn wheel and one drive wheel)
 */
class SwerveDrive{
    SwerveDrive(PID turnPID, PID drivePID){
        this->turnPID = turnPID;
        this->drivePID = drivePID;
    }

    void Setup(){
        this->turnPID.Setup();
        this->drivePID.Setup();
    }

    void SetPidTargets(float turnMotorPercentage, float wheelMotorPercentage){
        this->turnPID.updateTarget(turnMotorPercentage);
        this->drivePID.updateTarget(wheelMotorPercentage);
    }

    void TogglePids(bool value){
        this->turnPID.setActive(value);
        this->drivePID.setActive(value);
    }

    void ResetPids(){
        this->turnPID.reset();
        this->drivePID.reset();
    }
    
    private:
    PID turnPID;
    PID drivePID;
};