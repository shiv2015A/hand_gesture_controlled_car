/*
 * Hand Gesture Controlled RC Car - Arduino Receiver
 * 
 * Hardware:
 *   - Arduino Uno (or compatible)
 *   - L298N / Generic L293D Motor Driver Module 
 *   - 2 or 4 DC Motors
 *
 * Operation:
 *   Listens for bytes ('f', 'b', 'L', 'R', 's') on Hardware Serial (9600 bps)
 *   and maps these to differential steering logic on 4 raw output pins.
 */

// Define Motor Pins
// Left Side Motors
const int MOTOR_A_IN1 = 4;
const int MOTOR_A_IN2 = 5;
// Right Side Motors
const int MOTOR_B_IN3 = 6;
const int MOTOR_B_IN4 = 7;

void setup() {
  Serial.begin(9600); // Must match the Python script baud rate
  
  // Initialize driver pins as output
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  pinMode(MOTOR_B_IN3, OUTPUT);
  pinMode(MOTOR_B_IN4, OUTPUT);
  
  // Halt all motors on startup
  stopMotors();
  Serial.println("Arduino Online. Waiting for commands...");
}

void loop() {
  // Check if serial byte is available from Python CV script
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    // Evaluate byte incoming
    switch(command) {
      case 'f':
        driveForward();
        break;
      case 'b':
        driveBackward();
        break;
      case 'L':
        pivotLeft();
        break;
      case 'R':
        pivotRight();
        break;
      case 's':
        stopMotors();
        break;
      default:
        // Ignore unrecognized noise
        break;
    }
  }
}

// ------ Movement Functions ------

void driveForward() {
  digitalWrite(MOTOR_A_IN1, HIGH);
  digitalWrite(MOTOR_A_IN2, LOW);
  digitalWrite(MOTOR_B_IN3, HIGH);
  digitalWrite(MOTOR_B_IN4, LOW);
}

void driveBackward() {
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, HIGH);
  digitalWrite(MOTOR_B_IN3, LOW);
  digitalWrite(MOTOR_B_IN4, HIGH);
}

void pivotLeft() {
  // Left side moves back, Right side moves forward
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, HIGH);
  digitalWrite(MOTOR_B_IN3, HIGH);
  digitalWrite(MOTOR_B_IN4, LOW);
}

void pivotRight() {
  // Right side moves back, Left side moves forward
  digitalWrite(MOTOR_A_IN1, HIGH);
  digitalWrite(MOTOR_A_IN2, LOW);
  digitalWrite(MOTOR_B_IN3, LOW);
  digitalWrite(MOTOR_B_IN4, HIGH);
}

void stopMotors() {
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, LOW);
  digitalWrite(MOTOR_B_IN3, LOW);
  digitalWrite(MOTOR_B_IN4, LOW);
}
