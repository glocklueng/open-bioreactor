

void setup()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT); //LED
  pinMode(22, OUTPUT); 
  pinMode(23, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);  
}


void loop()
{
  Serial.println("New round!");
    Serial.println("PIN high."); 
  digitalWrite(13, HIGH);
  digitalWrite(22, HIGH); 
  digitalWrite(23, HIGH);
  digitalWrite(24, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(27, HIGH);
  delay(2000);
  Serial.println("PIN low.");
  digitalWrite(13, LOW);
  digitalWrite(22, LOW); 
  digitalWrite(23, LOW);
  digitalWrite(24, LOW);
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  delay(2000);
  Serial.println("PIN high.");  
  digitalWrite(13, HIGH);
  digitalWrite(22, HIGH); 
  digitalWrite(23, HIGH);
  digitalWrite(24, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(27, HIGH);
  delay(200);
  Serial.println("PIN low."); 
  digitalWrite(13, LOW);
  digitalWrite(22, LOW); 
  digitalWrite(23, LOW);
  digitalWrite(24, LOW);
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  delay(200);
  Serial.println("PIN high.");  
    digitalWrite(13, HIGH);
  digitalWrite(22, HIGH); 
  digitalWrite(23, HIGH);
  digitalWrite(24, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(27, HIGH);
  delay(5000);
    Serial.println("PIN low.");
  digitalWrite(13, LOW);
  digitalWrite(22, LOW); 
  digitalWrite(23, LOW);
  digitalWrite(24, LOW);
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  delay(1000);

}

