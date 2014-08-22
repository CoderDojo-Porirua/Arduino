int white = 13;
int button = 12;
int red = 11;
int orange = 10;
int green = 9;
int time = 0;
int reaction = 0;

void setup()
{
  pinMode(button, INPUT);
  pinMode(white, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(orange, OUTPUT);
  pinMode(green, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  digitalWrite(white, HIGH);
  delay(200);
  digitalWrite(white, LOW);
  delay(200);
  digitalWrite(white, HIGH);
  delay(200);
  digitalWrite(white, LOW);
  delay(200);
  digitalWrite(white, HIGH);
  delay(200);
  digitalWrite(white, LOW);
  delay(random(5000, 10000));
  digitalWrite(white, HIGH);
  time = millis();
  do {
    delay(1);
  } while (digitalRead(button) == LOW);
  digitalWrite(white, LOW);
  reaction = millis() - time;
  digitalWrite(red, HIGH);
  if (reaction < 500) {
    delay(1000);
    digitalWrite(orange, HIGH);
  }
  if (reaction < 250) {
    delay(1000);
    digitalWrite(green, HIGH);
  }
  Serial.println(reaction);
  delay(3000);
  digitalWrite(red, LOW);
  digitalWrite(orange, LOW);
  digitalWrite(green, LOW);
}
