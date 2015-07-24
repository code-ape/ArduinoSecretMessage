//led 0 - > Binary blink
//led 1 - > Binary Blink
//led 2 - > 1 second time
//led 3 - > Letter
//led 4 - > Sentance

//####//####//####//####//####//####//
// Edit the text between the quotes //
// below to add your secret message //
// to your device.                  //
//####//####//####//####//####//####//

char secret_string[ ] = "I love";     //initialize

//####//####//####//####//####//####//

int led[5] = {0, 1, 2, 3, 4};

// LED role assignments
const int LED_FADE = led[0];
const int LED_FADE2 = led[1];
const int LED_MESSAGE = led[2];
const int LED_BLINK = led[3];
const int LED_END_LETTER = led[4];
const int LED_END_MESSAGE = led[1];

// LED times to stay on
const int DELAY_BLINK = 2000;
const int LETTER_TIME = 2000;
const int DELAY_END_LETTER = 3000;
const int DELAY_END_MESSAGE = 5000;

// string variables
int stringLength = sizeof(secret_string);
int stringPosition = 0;
int binaryPosition = 7;

// state variable
int state = 0;

unsigned long led_time_register[5];

//state definitions
const int STARTING_MESSAGE = 1;
const int STARTING_LETTER = 2;
const int PRINTING_LETTER = 3;
const int END_OF_LETTER = 4;
const int INBETWEEN_LETTERS = 5;
const int END_OF_MESSAGE = 6;
const int INBETWEEN_MESSAGE = 7;

// time variables
int cycle_delay = 5; //milliseconds
unsigned long prior_loop_time = 0; //milliseconds
unsigned long TIME;
unsigned long loop_start;
unsigned long loop_end;
unsigned long letter_end_time;
unsigned long message_end_time;
unsigned long last_blink;

// blink variables
boolean blink_state = true; // true = on, false = off
const boolean ON = true;
const boolean OFF = false;


//setup
void setup()  {
  TIME = millis();
  //  Serial.begin(9600);
  //  Serial.println("Beginning setup");

  delay(500);
  for (int i = 0; i < 5; i++ ) {
    pinMode(led[i], OUTPUT);
    led_time_register[i] = 0;
    digitalWrite(led[i], HIGH);
  }
  delay(300);
  for (int i = 4; i >= 0; i--) {
    digitalWrite(led[i], LOW);
    delay(100);
  }
  delay(1000);

  state = STARTING_MESSAGE;

  //  Serial.println("Exiting setup");
}


char letter;
boolean bit;

// main loop
void loop() {

  prior_loop_time = loop_end - loop_start;
  loop_start = millis();

  if (prior_loop_time < 5) {
    delay(5 - prior_loop_time);
  }

  TIME += prior_loop_time;


  // turns blinking light on and off
  if (time_has_passed_since(DELAY_BLINK, last_blink)) {
    turn_led_to_for_dur(LED_BLINK, DELAY_BLINK / 2, ON);
    last_blink = TIME;
  }

  int fade_val = 127.5 + 127.5 * sin(2 * PI / 2500 * TIME);
  analogWrite(LED_FADE, fade_val);    //tell LED what its PWM fade value should be.

  if (state != INBETWEEN_MESSAGE) {
    int fade_val = 127.5 + 127.5 * sin(2 * PI / 2500 * TIME);
    analogWrite(LED_FADE, fade_val);
    analogWrite(LED_FADE2, 255 - fade_val);  //tell LED what its PWM fade value should be.
  } else {
    digitalWrite(LED_FADE2, HIGH);
    digitalWrite(LED_FADE, HIGH);
  }

  turn_off_expired_leds();

  switch (state) {
    case STARTING_MESSAGE:
      {
        letter_end_time = 0;
        message_end_time = 0;
        stringPosition = 0;

        // Serial.println("Changing state to 'STARTING_LETTER'");
        state = STARTING_LETTER;
      }
      break;


    case STARTING_LETTER:
      {
        binaryPosition = 7;

        // Serial.println("Changing state to 'PRINTING_LETTER'");
        state = PRINTING_LETTER;
      }
      break;

    case PRINTING_LETTER:
      {
        if (!time_has_passed_since(LETTER_TIME, letter_end_time)) {
          break;
        }

        letter = get_letter(secret_string, stringPosition);
        bit = get_bit(letter, binaryPosition);
        print_bit(bit);

        binaryPosition--;
        letter_end_time = last_blink;

        if (binaryPosition < 0) {
          // Serial.println("Changing state to 'END_OF_LETTER'");
          state = END_OF_LETTER;
        }
      }
      break;

    case END_OF_LETTER:
      {
        if (!time_has_passed_since(LETTER_TIME, letter_end_time)) {
          break;
        }
        print_bit(OFF);
        turn_led_on_for_dur(LED_END_LETTER, DELAY_END_LETTER);

        stringPosition += 1;

        if (stringPosition >= stringLength - 1) {
          //          Serial.println("Changing state to 'END_OF_MESSAGE'");
          state = END_OF_MESSAGE;
          message_end_time = TIME;
        } else {
          //          Serial.println("Changing state to 'INBETWEEN_LETTERS'");
          state = INBETWEEN_LETTERS;
          letter_end_time = TIME;
        }
      }
      break;


    case INBETWEEN_LETTERS:
      {
        if (time_has_passed_since(DELAY_END_LETTER, letter_end_time)) {
          //          Serial.println("Changing state to 'STARTING_LETTER'");
          state = STARTING_LETTER;
        }
      }
      break;

    case END_OF_MESSAGE:
      {
        turn_led_on_for_dur(LED_END_MESSAGE, DELAY_END_MESSAGE);

        //        Serial.println("Changing state to 'INBETWEEN_MESSAGE'");

        state = INBETWEEN_MESSAGE;

      }
      break;


    case INBETWEEN_MESSAGE:
      {
        if (time_has_passed_since(DELAY_END_MESSAGE, message_end_time)) {
          //          Serial.println("Changing state to 'STARTING_MESSAGE'");
          state = STARTING_MESSAGE;
        }
      }
      break;
  }

  loop_end = millis();
}



char get_letter(char *char_array, int position) {
  return char_array[position];
}

boolean get_bit(char letter, int position) {
  if (letter & 1 << position) {
    return true;
  } else {
    return false;
  }
}

void print_bit(boolean bit) {
  if (bit == ON) {
    //    Serial.println("Printing '1' bit");
  } else {
    //    Serial.println("Printing '0' bit");
  }

  if (bit == ON) {
    //digitalWrite(LED_MESSAGE, HIGH);
    turn_led_to_for_dur(LED_MESSAGE, LETTER_TIME / 2, ON);
  } else {
    //digitalWrite(LED_MESSAGE, LOW);
    turn_led_to_for_dur(LED_MESSAGE, LETTER_TIME / 2, OFF);
  }
}


void turn_led_on_for_dur(int led_number, int duration) {
  //  Serial.print("Turning on led '");
  //  Serial.print(led_number);
  //  Serial.print("' for '");
  //  Serial.print(duration);
  //  Serial.print("' milliseconds\n");
  turn_led_to_for_dur(led_number, duration, ON);
}

void turn_led_to_for_dur(int led_number, int duration, boolean state) {
  turn_led_to_for_dur_since(led_number, duration, state, TIME);
}


void turn_led_to_for_dur_since(int led_number, int duration, boolean state, unsigned long initial_time) {

  if (state == ON) {
    digitalWrite(led_number, HIGH);
  } else {
    digitalWrite(led_number, LOW);
  }

  led_time_register[led_number] = initial_time + duration;
}


boolean time_has_passed_since(int interval_length, unsigned long start_time) {
  if (start_time + ((unsigned long) interval_length) < TIME) {
    return true;
  } else {
    return false;
  }
}


void turn_off_expired_leds() {
  for (int pin = 0; pin < 5; pin++) {

    if ( led_time_register[pin] > 0 ) {
      if ( TIME > led_time_register[pin] ) {
        //        Serial.print("Turning off led '");
        //        Serial.print(pin);
        //        Serial.print("'\n");

        digitalWrite(pin, LOW);

        led_time_register[pin] = 0;
      }
    }

  }
}

