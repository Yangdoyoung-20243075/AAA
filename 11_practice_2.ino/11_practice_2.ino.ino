#include <Servo.h>

// 핀 설정
#define PIN_LED   9   // LED active-low
#define PIN_TRIG  12  // 초음파 센서 TRIGGER
#define PIN_ECHO  13  // 초음파 센서 ECHO
#define PIN_SERVO 10  // 서보 모터

// 초음파 및 거리 설정
#define SND_VEL 346.0     // 음속 (m/s)
#define INTERVAL 50        // 샘플링 간격 (ms)
#define PULSE_DURATION 10  // 초음파 펄스 지속 시간 (μs)
#define DIST_MIN_CM 18     // 최소 거리 (cm)
#define DIST_MAX_CM 36     // 최대 거리 (cm)

// 타이밍 및 거리 변환 계수
#define TIMEOUT ((INTERVAL / 2) * 1000.0) // 최대 에코 대기 시간 (μs)
#define SCALE (0.034 / 2.0)               // 거리 변환 계수 (cm)

// 글로벌 변수
unsigned long last_sampling_time = 0;
Servo myservo;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO); // 서보 모터 핀 연결
  myservo.write(0); // 초기 서보 위치 0도

  Serial.begin(57600); // 시리얼 통신 시작
}

void loop() {
  // 샘플링 간격이 지나지 않았으면 대기
  if (millis() < last_sampling_time + INTERVAL)
    return;

  // 거리 측정 (cm 단위)
  float distance = measureDistance();
  int angle = calculateServoAngle(distance); // 거리로 각도 계산

  // 서보 모터 각도 설정
  myservo.write(angle);

  // 시리얼 플로터 및 모니터에 데이터 출력
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Servo Angle: ");
  Serial.println(angle);

  // 시리얼 플로터용 형식 출력
  Serial.print(distance);
  Serial.print(",");
  Serial.println(angle);

  // LED 제어 (거리 범위 내에 있을 때 ON)
  if (distance >= DIST_MIN_CM && distance <= DIST_MAX_CM) {
    digitalWrite(PIN_LED, LOW);  // LED ON
  } else {
    digitalWrite(PIN_LED, HIGH); // LED OFF
  }

  // 샘플링 시간 갱신
  last_sampling_time = millis();
}

// 초음파 센서로 거리 측정 (cm 단위 반환)
float measureDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, TIMEOUT); // 왕복 시간 측정
  return duration * SCALE; // 거리 변환 (cm 단위)
}

// 거리에 따라 서보 모터 각도 계산
int calculateServoAngle(float distance) {
  if (distance <= DIST_MIN_CM) {
    return 0; // 최소 거리일 때 0도
  } else if (distance >= DIST_MAX_CM) {
    return 180; // 최대 거리일 때 180도
  } else {
    // 18cm ~ 36cm 사이에서 선형적으로 0도 ~ 180도로 변환
    return map(distance, DIST_MIN_CM, DIST_MAX_CM, 0, 180);
  }
}
