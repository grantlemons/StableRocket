#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <SD.h>
#include <Servo.h>
#include <Wire.h>

Servo red1;
Servo red2;
Servo blue1;
Servo blue2;

Adafruit_BNO055 gyro = Adafruit_BNO055(55);

class Rocket {
    public:
    int redPos;
    int bluePos;
    int setupRepeat = 0;
    int min = 0;
    int max = 180;
    int mid = (min + max)/2;
    //180 is up?
    int targetY = 87.75;
    int targetX = 0;
    //ratio of rocket angle to fin angle
    int divisor = 1;
    //Has it already completed
    boolean needReset = false;
    boolean set = false;
    //Timer for timestamp
    double startX;
    double startY;
    double X;
    double Y;

    void servoInit(int time) {
        // Initialisation
        red1.write(this->min);
        red2.write(this->min);
        blue1.write(this->min);
        blue2.write(this->min);
        delay(time*1000);

        red1.write(this->max);
        red2.write(this->max);
        blue1.write(this->max);
        blue2.write(this->max);
        delay(time*1000);

        red1.write(this->mid);
        red2.write(this->mid);
        blue1.write(this->mid);
        blue2.write(this->mid);
        delay(1000);
    }

    void writeToSD(String input) {
        File dataFile = SD.open("datalog.txt", FILE_WRITE);
        if (dataFile) {
            dataFile.println(input);
            dataFile.close();
        }
    }
};

Rocket rocket1;

void setup() {
    red1.attach(2);
    red2.attach(3);
    blue1.attach(4);
    blue2.attach(5);
    //pinMode(4, OUTPUT);
    rocket1.servoInit(1.5);
    Serial.begin(19200);

    if(gyro.begin()) {
        gyro.setExtCrystalUse(true);
    } else {
        while(1) {
            Serial.println("BNO055 Not Found");
            delay(1000);
        }
    }
    //digitalWrite(4, HIGH);
    Serial.println("setup complete");
}

void loop() {
    sensors_event_t event;
    gyro.getEvent(&event);

    if(!rocket1.set) {
        rocket1.startX = event.orientation.y;
        rocket1.startY = event.orientation.x;

        Serial.println(rocket1.startX);
        Serial.println(rocket1.startY);

        rocket1.set = true;
    }

    rocket1.X = event.orientation.x - rocket1.startX;
    rocket1.Y = event.orientation.y - rocket1.startY;

    //Actual Needed angle unknown
    //Currently set to opposite angle of rocket divided by 2
    //Negative if fins at top
    rocket1.redPos = -((rocket1.targetX-rocket1.X)/rocket1.divisor);
    rocket1.bluePos = -((rocket1.targetY-rocket1.Y)/rocket1.divisor);

    //Serial.println(event.orientation.x);
    //Serial.println(event.orientation.y);
    //Serial.println(event.orientation.z);
    Serial.println("");
    Serial.print(rocket1.redPos);
    Serial.print(", ");
    Serial.print(rocket1.bluePos);
    Serial.println("");
    //Serial.println("X:"+String(rocket1.X)+" Y:"+String(rocket1.Y)+" AZ:"+String(accel.z()));

    if(true) {
        red1.write(rocket1.redPos);
        red2.write(rocket1.redPos);
        blue1.write(rocket1.bluePos);
        blue2.write(rocket1.bluePos);
        //Serial.println("R: "+String(rocket1.redPos)+" B: "+String(rocket1.bluePos));
    }
    delay(100);
}