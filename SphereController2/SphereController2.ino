
// rotary encoder demo by 'jurs' for Arduino Forum
// Modified for push button
// A -> pinA + 10k -> GND
// B -> pinB + 10k -> GND
// +5V -> Center + Pin 1
// Pin 2 -> pushPin + 10k -> GND
// Output format {PIN} {+(up) -(down), !(press), X(release)}
// only gives up and down & lets the other end handle what to do with them
/*  Bytes
0000  Left-   0100  Right-
0001  Left+   0101  Right+
0010  Left!   0110  Right!
0011  LeftX   0111  RightX
*/ 

struct rotary_t {
  int pinA;
  int pinB;
  int touchPin;
  unsigned int count;
  int last;
  int lastTouch;
};


String feed;

rotary_t encoder[] = { // define 3 pins for each rotary encoder
           {2, 3, A0, 32768}, // encoder[0].pinA, encoder[0].pinB, encoder[0].touchPin
           {4, 5, A1, 32768},
           {6, 7, A2, 32768}, // encoder[0].pinA, encoder[0].pinB, encoder[0].touchPin
           {8, 9, A3, 32768},
                      };

#define NUMENCODERS (sizeof(encoder)/sizeof(encoder[0]))
#define MODVAL 678

volatile byte state_ISR[NUMENCODERS];
volatile int8_t count_ISR[NUMENCODERS];


void beginEncoders()
{ // active internal pullup resistors on each encoder pin and start timer2
  for (int i = 0; i < NUMENCODERS; i++)
  {
    pinMode(encoder[i].pinA, INPUT_PULLUP);
    pinMode(encoder[i].pinB, INPUT_PULLUP);
    pinMode(encoder[i].touchPin, INPUT);
    readEncoder(i); // Initialize start condition
  }
  startTimer2();
}

boolean updateEncoders()
{ // read all the 'volatile' ISR variables and copy them into normal variables
  boolean changeState = false;
  for (int i = 0; i < NUMENCODERS; i++)
  {
    if (count_ISR[i] != 0)
    {
      changeState = true;
      noInterrupts();
      encoder[i].count += count_ISR[i];
      count_ISR[i] = 0;
      interrupts();
    }
  }
  return changeState;

  Serial.print("-");
}

void printEncoders()
{ // print current count of each encoder to Serial
  unsigned int moded;
  for (int i = 0; i < NUMENCODERS; i++)
  {
    if (encoder[i].last != encoder[i].count) {
        /* Just to test least significant digit from float conversion
         *  Answer was 4
        int dif = encoder[i].last - encoder[i].count;
        int difmo = dif % MODVAL;
        float diff = (float)difmo / (float)MODVAL;
        */
        
        encoder[i].last = encoder[i].count;
        moded = encoder[i].count % MODVAL;
        float normed = (float)moded / (float)MODVAL;
        // Serial.println((String)i + "," + String(normed, 4)); // + "\t" + String(diff, 6));
        Serial.println((String)i + "," + String(encoder[i].count)); // + "\t" + String(diff, 6));
        
    }
  }
}

int8_t readEncoder(byte i)
{ // this function is called within timer interrupt to read one encoder!
  int8_t result = 0;
  byte state = state_ISR[i];
  state = state << 2 | (byte)digitalRead(encoder[i].pinA) << 1 | (byte)digitalRead(encoder[i].pinB);
  state = state & 0xF;  // keep only the lower 4 bits
  // next two lines would be code to read 'quarter steps'
  if (state == 0b0001 || state == 0b0111 || state == 0b1110 || state == 0b1000) result = -1;
  else if (state == 0b0010 || state == 0b1011 || state == 0b1101 || state == 0b0100) result = 1;

  // next two lines is code to read 'full steps'
  if (state == 0b0001) result = -1;
  else if (state == 0b0010) result = 1;
  state_ISR[i] = state;
  return result;
}


void startTimer2()  // start TIMER2 interrupts
{
  noInterrupts();
  // Timer 2 CTC mode
  TCCR2B = (1 << WGM22) | (1 << CS22)  | (1 << CS20);
  TCCR2A = (1 << WGM21);
  OCR2A = 124;   // 249==500,  124==1000 interrupts per second
  // 63 ==2000,  31==4000
  // 15 ==8000,   7==16000
  TIMSK2 = (1 << OCIE2A); // enable Timer 2 interrupts
  interrupts();
}

void stopTimer2() // stop TIMER2 interrupts
{
  noInterrupts();
  TIMSK2 = 0;
  interrupts();
}


ISR(TIMER2_COMPA_vect)  // handling of TIMER2 interrupts
{
  for (int i = 0; i < NUMENCODERS; i++)
  {
    count_ISR[i] += readEncoder(i);
  }
}


#define BAUDRATE 9600L // serial baud rate

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("Connect!"); // print some Test-Message at beginning
  beginEncoders();

}

int touch; 
void loop() {
  if (updateEncoders()) printEncoders();

  /*
  for (int i = 0; i < 1; i++) {
    
    touch = analogRead(encoder[i].touchPin);
    if (touch != encoder[i].lastTouch){  
      encoder[i].lastTouch = touch;   
      Serial.println((String)i+10 + "," + touch);
    }
    //Serial.println((String)i + ": " + (String)encoder[i].count + "\t" + (String)touch); 
  }*/
}
