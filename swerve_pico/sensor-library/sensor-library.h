class ZeroingSensor
{
public:
    ZeroingSensor(int pin);
    void Setup();
    int Read();

private:
    int readPin;
};