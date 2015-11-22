//led 0 - > Fade / Sentance
//led 1 - > Fade 180 / Sentance
//led 2 - > Binary
//led 3 - > 1 second
//led 4 - > 8bit indicator

//####//####//####//####//####//####//
// Edit the text between the quotes //
// below to add your secret message //
// to your device.                  //
//####//####//####//####//####//####//

char *string = "Hi";

//####//####//####//####//####//####//

int led0 = 0;
int led1 = 1;
int led2 = 2;
int led3 = 3;
int led4 = 4;

int led3del = 1000;
int led3state = 0;
int led4del = 3000;
int led0del = 5000;

int nowTime = 0;
int lastTime = 0;

int value;      // variable to hold output from cos function
int pd = 2500;  // peroid of led0 fade


void setup()  {
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
}



int i = 7;
int stringLength = strlen(string);
int stringCount = 0;
char *string2 = string;

void loop()  {


  nowTime = millis();                       //get the current amount of time the microcontroller has been on

  value = 127.5 + 127.5 * sin(2 * PI / pd * nowTime); // calculate fade PWM percentage
  analogWrite(led0, value);                       //tell LED what its PWM fade value should be.
  value = 127.5 + 127.5 * sin((2 * PI / pd * nowTime) + PI); // calculate fade PWM percentage
  analogWrite(led1, value);                       //tell LED what its PWM fade value should be.

  if ((nowTime - lastTime) >= led3del) {    //test to see if enough time has passed to blink
    if (led3state == 0) {                   //determine if led3 is off
      digitalWrite(led3, HIGH);             //turn led3 on
      led3state = 1;                        //change the state so the program knows led3 is on
      if (*string & 1 << i) {               //read the first value of the 8 bit character
        digitalWrite(led2, HIGH);
      } else {
        digitalWrite(led2, LOW);
      }
      if (i >= 0) {                         //as long as the incrimenter is above 0, reduce it by 1
        --i;
      } else {
        digitalWrite(led2, LOW);            //if the incirmenter is >=0, turn led 2, 3 off
        digitalWrite(led3, LOW);
        i = 7;                              //reset the incrimenter to 7
        *string++;                          //move to the next letter in the string you created
        stringCount++;                      //incriment the counter that tells when you reach the end of the string
        digitalWrite(led4, HIGH);           //turn led4 on
        digitalWrite(led0, LOW);            //turn led0 off
        digitalWrite(led1, LOW);            //turn led1 off
        delay(led4del);                     //delay for led4, tells that you are at the end of a letter
        if (stringCount >= stringLength) {  //this checks to see if you are at the end of the string
          digitalWrite(led0, HIGH);         //if you are at the end, turn led0 on
          digitalWrite(led1, HIGH);         //if you are at the end, turn led1 on
          digitalWrite(led2, HIGH);         //if you are at the end, turn led2 on
          digitalWrite(led3, HIGH);         //if you are at the end, turn led3 on
          digitalWrite(led4, HIGH);         //if you are at the end, turn led4 on
          delay(led0del);                   //pause to give time before resetting the sentance
          string = string2;                 //reset the string to its original value
          stringCount = 0;                  //reset the string counter
          digitalWrite(led0, LOW);          //turn off led0 and start over.
          digitalWrite(led1, LOW);         //if you are at the end, turn led1 off
          digitalWrite(led2, LOW);         //if you are at the end, turn led2 off
          digitalWrite(led3, LOW);         //if you are at the end, turn led3 off
          digitalWrite(led4, LOW);         //if you are at the end, turn led4 off
        }
      }

    } else {
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      led3state = 0;                        //update the state so the program knows the leds are off
    }

    lastTime = nowTime;                     //update the time for the loop, so it knows how much time has passed
  }

}
