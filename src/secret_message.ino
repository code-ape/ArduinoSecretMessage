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

char secret_string[ ] = "This is my message";     //initialize

//####//####//####//####//####//####//

const int led0 = 0;
const int led1 = 1;
const int led2 = 2; // 1 second pulse
const int led3 = 3; // end of letter
const int led4 = 4; // end of message


// LED role assignments
const int LED_FADE = led0;
const int LED_MESSAGE = led1;
const int LED_BLINK = led2;
const int LED_END_LETTER = led3;
const int LED_END_MESSAGE = led4;

// LED times to stay on
const int DELAY_BLINK = 1000;
const int DELAY_END_LETTER = 3000;
const int DELAY_END_MESSAGE = 5000;

// string variables
int stringLength = sizeof(secret_string);
int stringPosition = 0;
int binaryPosition = 7;

// register used to leave lights on for desired duration
// if duration is 0 then it is considered to be off.
typedef struct {
  unsigned long start_time;
  int duration;
} LED_State;

LED_State led_time_register[5];

//state definitions
const int STARTING_MESSAGE = 1;
const int STARTING_LETTER = 2;
const int PRINTING_LETTER = 3;
const int END_OF_LETTER = 4;
const int INBETWEEN_LETTERS = 5;
const int END_OF_MESSAGE = 6;
const int INBETWEEN_MESSAGE = 7;

// state variable
int state;

// time variables
int cycle_delay = 5; //milliseconds
int prior_loop_time = 0; //milliseconds
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
  Serial.println("Beginning setup");
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  for (int i = 0; i < 5; i++ ) {
    led_time_register[i] = (LED_State) {0,0};
  }

  state = STARTING_MESSAGE;

  Serial.println("Exiting setup");
}


char letter;
boolean bit;

// main loop
void loop() {

  unsigned long prior_loop_time = loop_end - loop_start;

  if (prior_loop_time < 5) {
    delay(5 - prior_loop_time);
  }

  loop_start = millis();

  // turns blinking light on and off
  if (time_has_passed_since(DELAY_BLINK, last_blink)) {
    if (blink_state == ON) {
      Serial.println("Turning blinking light off");
      digitalWrite(LED_BLINK, LOW);
      blink_state = OFF;
    } else {
      Serial.println("Turning blinking light on");
      digitalWrite(LED_BLINK, HIGH);
      blink_state = ON;
    }
    last_blink = millis();
  }

  turn_off_expired_leds();

  switch (state) {
    case STARTING_MESSAGE:
      {
        letter_end_time = 0;
        message_end_time = 0;
        stringPosition = 0;

        Serial.println("Changing state to 'STARTING_LETTER'");
        state = STARTING_LETTER;
      }
      break;


    case STARTING_LETTER:
      {
        binaryPosition = 7;

        Serial.println("Changing state to 'PRINTING_LETTER'");
        state = PRINTING_LETTER;
      }
      break;

    case PRINTING_LETTER:
      {
        letter = get_letter(secret_string, stringPosition);
        bit = get_bit(letter, binaryPosition);
        print_bit(bit);

        binaryPosition -= 1;

        if (binaryPosition < 0) {
          Serial.println("Changing state to 'END_OF_LETTER'");
          state = END_OF_LETTER;
        }
      }
      break;

    case END_OF_LETTER:
      {
        turn_led_on_for_dur(LED_END_LETTER, DELAY_END_LETTER);

        stringPosition += 1;

        if (stringPosition >= stringLength) {
          Serial.println("Changing state to 'END_OF_MESSAGE'");
          state = END_OF_MESSAGE;
          message_end_time = millis();
        } else {
          Serial.println("Changing state to 'INBETWEEN_LETTERS'");
          state = INBETWEEN_LETTERS;
          letter_end_time = millis();
        }
      }
      break;


    case INBETWEEN_LETTERS:
      {
        if (time_has_passed_since(DELAY_END_LETTER, letter_end_time)) {
          Serial.println("Changing state to 'STARTING_LETTER'");
          state = STARTING_LETTER;
        }
      }
      break;

    case END_OF_MESSAGE:
      {
        Serial.println("Changing state to 'INBETWEEN_MESSAGE'");
        state = INBETWEEN_MESSAGE;
      }
      break;


    case INBETWEEN_MESSAGE:
      {
        if (time_has_passed_since(DELAY_END_MESSAGE, message_end_time)) {
          Serial.println("Changing state to 'STARTING_MESSAGE'");
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
  if (letter & 1 << position){
    return true;
  } else {
    return false;
  }
}

void print_bit(boolean bit) {
  if (bit == ON) {
    digitalWrite(LED_MESSAGE, HIGH);
  } else {
    digitalWrite(LED_MESSAGE, LOW);
  }
}


void turn_led_on_for_dur(int led_number, int duration) {
  Serial.print("Turning on led '");
  Serial.print(led_number);
  Serial.print("' for '");
  Serial.print(duration);
  Serial.print("' milliseconds\n");

  digitalWrite(led_number, HIGH);

  led_time_register[led_number].duration = duration;
  led_time_register[led_number].start_time = millis();
}


boolean time_has_passed_since(int interval_length, unsigned long start_time) {
  if (start_time + ((unsigned long) interval_length) < millis()) {
    return true;
  } else {
    return false;
  }
}


void turn_off_expired_leds() {
  for (int pin = 0; pin < 5; pin++) {
    float duration = (unsigned long) led_time_register[pin].duration;

    if ( duration > 0 ) {
      if ( millis() > led_time_register[pin].start_time + duration ) {
        Serial.print("Turning off led '");
        Serial.print(pin);
        Serial.print("'\n");

        digitalWrite(pin, LOW);

        led_time_register[pin] = (LED_State) {0,0};
      }
    }

  }
}
