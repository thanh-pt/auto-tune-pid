#include <SoftwareSerial.h>
#include <stdio.h>

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

char result[30] = "";

class pid_system {
  private:
    float m_Kp, m_Ki, m_Kd;

    int m_setPoint = 0;

    int m_previous_error = 0;
    int m_error = 0;

    float m_output = 0;

    float m_dt = 0.1;

    float m_integral = 0;
    float m_derivative = 0;
  public:
    pid_system(){};
    pid_system(float Kp, float Ki, float Kd, float dt, int SetPoint)
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
      m_previous_error = 0;
      m_error = 0;
    }
    void setSetpoint(int SetPoint){
      m_setPoint = SetPoint;
    }
    void setDt(float dt){
      m_dt = dt;
    }
  public:
    float getOutput(){
      return m_output;
    }
    int getSetPoint(){
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
  bool isRun = false;
  void setPin(int pin){
    m_control_pin = pin;
    pinMode(m_control_pin, OUTPUT);
  }
  void convert(float& output){
    output = round(output);
  }
  void turnOff(){
    analogWrite(m_control_pin, 0);
  }
  void control(){
    int a = round(getOutput());
    analogWrite(m_control_pin, a);
  }
};

motor_control m1;
SoftwareSerial bluetooth(RX_PIN, TX_PIN);
int baudRate[] = {300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

void excurseCmd(char str[80]){
  Serial.println(str);
  int cmd = 0;
  int p, i, d;
  sscanf(str, "%d %d %d %d", &cmd, &p, &i, &d);
  switch (cmd){
    case 0:
    m1.isRun = false;
    break;
    case 1:
    m1.isRun = true;
    break;
    case 2:
    m1.setSetpoint((float)p);
    break;
    case 3:
    m1.isRun = false;
    m1.setupPID((float)p / 100, (float)i/100, (float)d/100);
    m1.isRun = true;
    break;
    case 4:
    break;
    case 5:
    break;
    }
}

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
     bluetooth.print("AT+BAUD8\t\n");
     bluetooth.flush();
     delay(100);
  }
  bluetooth.begin(115200);
  Serial.println("Config done, wait for ready...");
  while (!bluetooth) {}
  Serial.println("Bluetooth is ready!");
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
    if (m1.isRun){
      m1.update(counter);
    } else {
      m1.turnOff();
    }
    // TODO: sent the data to the phone
    sprintf(result, "%d %d\n", m1.getSetPoint(), counter);
    bluetooth.write(result);
    Serial.println(result);

    // Reset counter:
    counter = 0;
  }

  if (bluetooth.available()) {
    char receiveStr[80] = "";
    int len = 0;
    while (bluetooth.available()){
      receiveStr[len++] = (char)bluetooth.read();
    }
    excurseCmd(receiveStr);
  }
}
