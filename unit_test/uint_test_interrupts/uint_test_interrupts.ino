void setup() {  
  PCICR |= B00000100;      //Bit2 = 1 -> "PCIE0" enabeled (PCINT0 to PCINT7)
  PCMSK2 |= B10000000;      //Bit5 = 1 -> "PCINT8" enabeled -> A15 will trigger interrupt


  pinMode(A15, INPUT_PULLUP);    // sets the digital pin 13 as output
  Serial.begin(9600);
}

int x=0;
void loop() { 
  //your code here...

}

ISR (PCINT2_vect) 
{
    // For PCINT of pins D0 a D7
    Serial.print("Hello World ");
    Serial.println(x);
    x+=1;
} 
