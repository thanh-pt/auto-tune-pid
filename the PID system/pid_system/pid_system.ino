#define encoder_input_a 6
#define encoder_input_b 7
#define motor_signal_control 5

#define MAX_OUTPUT_CONTROL 255

// varible of timer
unsigned long DELAY_TIME = 100; // (ms)
unsigned long delay_start = 0; // the time the delay started
bool delay_running = false; // true if still waiting for delay to finish

// varible of counter
int counter = 0;
int encoder_input_a_state;
int encoder_input_a_last_state;

//varible of PID
float previous_error = 0;
float error = 0;
float integral = 0;
float derivative = 0;
float setpoint = 0;
float measured_value = 0;
float dt = 0;
float Ku, Tu;
float Kp, Ki, Kd;
float output = 0;
int output_control = 0;

void setup() {
  pinMode (encoder_input_a, INPUT);
  pinMode (encoder_input_b, INPUT);
  pinMode (motor_signal_control, OUTPUT);

  Serial.begin (9600);
  encoder_input_a_last_state = digitalRead(encoder_input_a);

  // start delay
  delay_start = millis();
  delay_running = true;
  // setup PID
//  Kp = 0.23;
//  Ki = 0.08;
//  Kd = 0;
  Ku = 0.10;
  Tu = 0.45;
//  //P:
//  Kp = Ku / 2;
//  PI:
  Kp = 0.45 * Ku;
  Ki = 1.2 * Kp / Tu;
  //PID:
//  Kp = Ku * 0.6;
//  Ki = 1.2 * Kp / Tu;
//  Kd = 3 * Kp * Tu / 40;
  dt = 0.1;
  setpoint = 2000;
}
void loop() {
  // Control motor
  analogWrite(motor_signal_control, output_control);
  // Read the encoder signal
  encoder_input_a_state = digitalRead(encoder_input_a);
  if (encoder_input_a_state != encoder_input_a_last_state) {
    counter ++;
//    if (digitalRead(encoder_input_b) != encoder_input_a_state) {
//      counter ++;
//    } else {
//      counter --;
//    }
  }
  encoder_input_a_last_state = encoder_input_a_state;
  
  // check if delay has timed out
  if (delay_running && ((millis() - delay_start) >= DELAY_TIME)) {
    delay_start += DELAY_TIME;
    
    // PID measure:
    measured_value = counter;
    
    error = setpoint - measured_value;
    if (output < MAX_OUTPUT_CONTROL || error < 0){
      integral = integral + error * dt;
    }
    derivative = (error - previous_error) / dt;
    
    output = Kp * error + Ki * integral + Kd * derivative;
    
    previous_error = error;
    

    output_control = round(limitValue(output, MAX_OUTPUT_CONTROL, 0));
    
    // Comunication:
    Serial.print(setpoint);
    Serial.print(" ");
    Serial.print(counter);
    Serial.println();

    // Reset counter:
    counter = 0;
  }
}

float limitValue(float value, float max_value, float min_value){
  float ret = value > max_value ? max_value : value;
  ret = ret > 0 ? ret : 0;
  return ret;
}
