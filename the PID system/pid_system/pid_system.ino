#include <SoftwareSerial.h>

#define TX_PIN      12
#define RX_PIN      11
#define BLUETOOTH_VCC 9
#define BLUETOOTH_GND 10

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

char result[10] = "";

class pid_system {
  private:
    float m_Kp, m_Ki, m_Kd;

    float m_setPoint = 0;

    float m_previous_error = 0;
    float m_error = 0;

    float m_output = 0;

    float m_dt = 0.1;

    float m_integral = 0;
    float m_derivative = 0;
  public:
    pid_system(){};
    pid_system(float Kp, float Ki, float Kd, float dt, float SetPoint)
    : m_Kp(Kp)
    , m_Ki(Ki)
    , m_Kd(Kd)
    , m_dt(dt)
    , m_setPoint(SetPoint){
    };
  public:
    void setupPID(float Kp, float Ki, float Kd){
      m_Kp = Kp;
      m_Ki = Ki;
      m_Kd = Kd;
    }
    void setSetpoint(float SetPoint){
      m_setPoint = SetPoint;
    }
    void setDt(float dt){
      m_dt = dt;
    }
  public:
    float getOutput(){
      return m_output;
    }
    float getSetPoint(){
      return m_setPoint;
    }
  public:
    void updateMeasuredValue(float measured_value){
      m_error = m_setPoint - measured_value;
      m_integral = m_integral + m_error * m_dt;
      m_derivative = (m_error - m_previous_error) / m_dt;
      
      m_output = m_Kp * m_error + m_Ki * m_integral + m_Kd * m_derivative;
      
      m_previous_error = m_error;
    }
    virtual void convert(float& output) = 0;
    virtual void control() = 0;

    void update(float measured_value){
      updateMeasuredValue(measured_value);
      convert(m_output);
      control();
    };
};

class motor_control : public pid_system {
  private:
    int m_control_pin = 5;
  public:
  void setPin(int pin){
    m_control_pin = pin;
    pinMode(m_control_pin, OUTPUT);
  }
  void convert(float& output){
    output = round(output);
  }
  void control(){
    int a = round(getOutput());
    //analogWrite(m_control_pin, a);
  }
};

motor_control m1;
SoftwareSerial bluetooth(RX_PIN, TX_PIN);
int baudRate[] = {300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

void setup() {
  pinMode (encoder_input_a, INPUT);
  pinMode (encoder_input_b, INPUT);

  Serial.begin (9600);
  encoder_input_a_last_state = digitalRead(encoder_input_a);

  // Config bluetooth
  pinMode(BLUETOOTH_VCC, OUTPUT);
  pinMode(BLUETOOTH_GND, OUTPUT);
  digitalWrite(BLUETOOTH_VCC, HIGH);
  digitalWrite(BLUETOOTH_GND, LOW);
  while (!Serial) {}
  
  Serial.println("Configuring, please wait...");
  
  for (int i = 0 ; i < 9 ; i++) {
     bluetooth.begin(baudRate[i]);
     String cmd = "AT+BAUD4";
     bluetooth.print("AT+BAUD4\t\n");
     bluetooth.flush();
     delay(100);
  }
  
  bluetooth.begin(9600);
  Serial.println("Config done");
  while (!bluetooth) {}
  
  Serial.println("Enter AT commands:");
  // start delay
  delay_start = millis();
  delay_running = true;

  // setup PID
  m1.setupPID(0.045, 0.12, 0);
  m1.setDt(0.1);
  m1.setSetpoint(2000);
  m1.setPin(motor_signal_control);
}

void loop() {
  // Read the encoder signal
  encoder_input_a_state = digitalRead(encoder_input_a);
  if (encoder_input_a_state != encoder_input_a_last_state) {
    counter ++;
  }
  encoder_input_a_last_state = encoder_input_a_state;
  
  // check if delay has timed out
  if (delay_running && ((millis() - delay_start) >= DELAY_TIME)) {
    delay_start += DELAY_TIME;
    
    // PID measure:
    m1.update(counter);
    // Serial.print(" ");
    // Serial.print(m1.getSetPoint());
    // Serial.print(" ");
    // Serial.print(counter);
    // Serial.print(" ");
    // Serial.println(0);
    sprintf(result, "%d\t\n", counter);
    bluetooth.write(result);

    // Reset counter:
    counter = 0;
  }
  if (bluetooth.available()) {
    Serial.write(bluetooth.read());
  }
  if (Serial.available()) {
    // bluetooth.write(Serial.read());
  }
}
