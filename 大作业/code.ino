  const int analogPin = A0; // 模拟输入引脚
  const float referenceVoltage = 5.0; // Arduino的参考电压（单位：伏特）

  class TestPoint;
  bool Diff(double V1, double V2);
  double GetU(double V1, double V2);
  double GetR(TestPoint TP1, TestPoint TP3);
  double GetC(TestPoint TP1, TestPoint TP3);
  double GetNPN(TestPoint TP1, TestPoint TP2, TestPoint TP3);
  double GetPNP(TestPoint TP1, TestPoint TP2, TestPoint TP3);
  bool isNPN(TestPoint TP1, TestPoint TP2, TestPoint TP3);
  bool isPNP(TestPoint TP1, TestPoint TP2, TestPoint TP3);
  double U(double V);
  bool isCorR(TestPoint TP1, TestPoint TP2, TestPoint TP3);
  void decide_element(TestPoint TP1, TestPoint TP2, TestPoint TP3);
  void draw(TestPoint TP1, TestPoint TP2, TestPoint TP3);
  double GetL(TestPoint TP1, TestPoint TP2, double resistance);
  void describGraph(TestPoint TP1, TestPoint TP2, TestPoint TP3);

  class TestPoint
  {
    public:
      TestPoint(int R1_pin, int R2_pin, int ADC_Pin)
      {
        this -> R1_pin = R1_pin;
        this -> R2_pin = R2_pin;
        this -> ADC_Pin = ADC_Pin;
        pinMode(R1_pin, OUTPUT);
        pinMode(R2_pin, OUTPUT);
        pinMode(ADC_Pin, INPUT);
      }
      void SetState(int TPState)//设置TestPoint状态,1 for HIGH_R1,2 for LOW_R1, 3 for HIGH_R2, 4 for LOW_R2, 5 for HIGH_R0, 6 for LOW_R0
      {
        switch(TPState)
        {
          case(1):    //HIGH_R1
          {
            pinMode(R1_pin, OUTPUT);
            digitalWrite(R1_pin, HIGH);
            pinMode(R2_pin, INPUT);
            pinMode(ADC_Pin,INPUT);
            break;
          }
          case(2):      //LOW_R1
          {
            pinMode(R1_pin,OUTPUT);
            digitalWrite(R1_pin,LOW);
            pinMode(R2_pin,INPUT);
            pinMode(ADC_Pin,INPUT);
            break;
          }
          case(3):      //HIGH_R2
          {
            pinMode(R1_pin, INPUT);
            pinMode(R2_pin, OUTPUT);
            digitalWrite(R2_pin, HIGH);
            pinMode(ADC_Pin, INPUT);
            break;
          }
          case(4):      //LOW_R2
          {
            pinMode(R1_pin,INPUT);
            pinMode(R2_pin,OUTPUT);
            digitalWrite(R2_pin, LOW);
            pinMode(ADC_Pin,INPUT); 
            break;
          }
          case(5):      //HIGH_R0
          {
            pinMode(R1_pin, INPUT);
            pinMode(R2_pin, INPUT);
            pinMode(ADC_Pin, OUTPUT);
            digitalWrite(ADC_Pin, HIGH);
            break;
          }
          case(6):      //LOW_R0
          {
            pinMode(R1_pin, INPUT);
            pinMode(R2_pin, INPUT);
            pinMode(ADC_Pin, OUTPUT);
            digitalWrite(ADC_Pin, LOW);
            break;
          }
        }

      }

      void SetStateDigital(int number)
      {
        pinMode(R1_pin, INPUT);
        pinMode(R2_pin, OUTPUT);
        // digitalWrite(R2_pin, number);
        analogWrite(R2_pin, number);
        pinMode(ADC_Pin, INPUT);
      }

      int analogRead()//读取TP电压ADC原始值，范围0-1023
      {
        return :: analogRead(ADC_Pin);
      }
      double ReadVoltage()//读取TP电压值，范围0-5V
      {
        int rawValue = analogRead();
        return rawValue * (5.0 / 1023.0);
      }
    private:
      int R1_pin, R2_pin, ADC_Pin;
  };

  //TP1,TP2,TP3实例化
  TestPoint TP1(2,3,A0);
  TestPoint TP2(4,5,A1);
  TestPoint TP3(6,7,A2);


  void setup() {
    Serial.begin(9600); // 初始化串口通信
  }

  double GetU(double V1, double V2){
    if (V1 > V2) return V1 - V2;
    else return V2 - V1; 
  }

  double U(double V){
    if (V<=0) return -V;
    else return V;
  }

  double GetR(TestPoint TP1, TestPoint TP3){
    TP1.SetState(1);
    TP3.SetState(2);
    double R = 1000.0;
    // double R = 10000.0;
    double V1 = TP1.ReadVoltage();
    double V3 = TP3.ReadVoltage();
    double Vx = GetU(V1, V3);
    double Rx;
    Rx = Vx * 2 * R /(5 - Vx);
    return Rx;
  }

  double GetC(TestPoint TP1, TestPoint TP3){
    TP1.SetState(2);
    TP3.SetState(2);
    double V1 = TP1.ReadVoltage();
    double V3 = TP3.ReadVoltage();
    double Vx = GetU(V1, V3);
    while (Vx > 0.1){
      V1 = TP1.ReadVoltage();
      V3 = TP3.ReadVoltage();
      Vx = GetU(V1, V3);
    }
    TP1.SetState(1);
    TP3.SetState(2);
    double timeStart = micros();
    while(Vx < 0.632*5){
      V1 = TP1.ReadVoltage();
      V3 = TP3.ReadVoltage();
      // Serial.println(V1);
      // Serial.println(V3);
      Vx = GetU(V1, V3);
    }
    double timeEnd = micros();
    double time = timeEnd - timeStart;
    double Cx = time / 2 / 1000;
    return Cx;
  }

  bool Diff(double V1, double V2){
    if (V1 - V2 < 0.3 && V1 - V2 > -0.3) return true;
    else if (V2 - V1 < 0.3 && V2 - V1 > -0.3) return true;
    return false;
  }

  double GetL(TestPoint TP1, TestPoint TP2, double resistance){
    // TP1.SetState(2);
    // TP2.SetState(2);
    // // delay(10);
    // double V1 = TP1.ReadVoltage();
    // double V2 = TP2.ReadVoltage();
    // double Vx = GetU(V1, V2);
    // // double initialVoltage = V2;

    // double timeStart = micros();
    // TP1.SetState(1);
    // while (!Diff(Vx, 5.0/3)){
    //   V1 = TP1.ReadVoltage();
    //   V2 = TP2.ReadVoltage();
    //   Vx = GetU(V1, V2);
    // // double initialVoltage = V2;
    // }
    // double timeEnd = micros();
    // double time  = timeEnd - timeStart;
    // Serial.println(time);

    TP1.SetState(4);
    TP2.SetState(4);
    delay(1000);

    double timeStart = micros();
    TP1.SetState(3);
    double V1 = TP1.ReadVoltage();
    double V2 = TP2.ReadVoltage();
    double Vx = GetU(V1, V2);
    while (Vx > 2.5){
      V1 = TP1.ReadVoltage();
      V2 = TP2.ReadVoltage();
      Vx = GetU(V1, V2);
    //  Serial.println(Vx);
    }
    double timeEnd = micros();
    double time = timeEnd - timeStart;
    // Serial.println(time);
    // return time;
    // while(Vx > 0.1){ // 等待电压变化
    //   V1 = TP1.ReadVoltage();
    //   V2 = TP2.ReadVoltage();
    //   Vx = GetU(V1, V2);
    // }
    // TP1.SetState(1); // 开始改变电感器两端的电压
    // initialVoltage = V2;
    // double Voltage = TP2.ReadVoltage();
    // double timeStart = micros();
    // while((5-V1)/1000 - 0.0025 < 0.1|| V2 / 1000 - 0.0025 < 0.1){ // 等待电压变化
    // double time; // 计算时间变化
    // while(TP2.ReadVoltage() > Voltage / 2.718) {}
    // double timeEnd = micros();
    // double currentchange = (newVoltage - initialVoltage) / resistance;
    // double time = timeEnd - timeStart; // 计算时间变化
    double inductance = time * 2 * resistance / 0.693147 / 1000000.0;
    // double inductance = time * resistance;
    // double inductance = (newVoltage * time) / currentchange;
    // double time = timeEnd - timeStart; // 计算时间变化
    // double Lx = time / resistance; // 电感值计算公式 L = T / R
    return inductance;
  }

  double GetNPN(TestPoint TP1, TestPoint TP2, TestPoint TP3){
    TP1.SetState(1);
    TP2.SetState(3);
    TP3.SetState(2);
    double Vb=U(TP2.ReadVoltage());
    double Ve=U(TP3.ReadVoltage());
    double Rc = 1000;
    double Re = 1000;
    double Rb = 680000;
    double Ib = (5-Vb)/Rb;
    double Ie = Ve/Re;
    double beta = Ie/Ib;
    Serial.print("Ib:");
    Serial.print(1000.0*Ib,3);
    Serial.print("mA  ");
    Serial.print("Ie:");
    Serial.print(1000.0*Ie,3);
    Serial.print("mA  ");
    Serial.print("beta:");
    Serial.println(beta,3);
  }

  double GetPNP(TestPoint TP1, TestPoint TP2, TestPoint TP3){
    TP1.SetState(2);
    TP2.SetState(4);
    TP3.SetState(1);
    double Vb=U(TP2.ReadVoltage());
    double Ve=U(TP3.ReadVoltage());
    double Rc = 1000;
    double Re = 1000;
    double Rb = 680000;
    double Ib=Vb/Rb;
    double Ie=(5-Ve)/Re;
    double beta=Ie/Ib;
    Serial.print("Ib:");
    Serial.print(1000.0*Ib,3);
    Serial.print("mA  ");
    Serial.print("Ie:");
    Serial.print(1000.0*Ie,3);
    Serial.print("mA  ");
    Serial.print("beta:");
    Serial.println(beta,3);
  }

  bool isNPN(TestPoint TP1, TestPoint TP2, TestPoint TP3){
    TP1.SetState(1);
    TP2.SetState(3);
    TP3.SetState(2);
    double V2 = TP2.ReadVoltage();
    double V3 = TP3.ReadVoltage();
    double V23=GetU(V2,V3);
    if (V23<0.8) return true;
    else return false;
  }

  bool isPNP(TestPoint TP1, TestPoint TP2, TestPoint TP3){
    TP1.SetState(2);
    TP2.SetState(4);
    TP3.SetState(1);
    double V2 = U(TP2.ReadVoltage());
    double V3 = U(TP3.ReadVoltage());
    double V23=GetU(V2,V3);
    if (V23<0.8) return true;
    else return false;
  }

  bool isCorR(TestPoint TP1, TestPoint TP2, TestPoint TP3){
    int i;
    TP1.SetState(1);
    TP3.SetState(2);
    double V[100];
    double sum = 0;
    for (i = 0; i < 100; i++){
      double V_TP1=U(TP1.ReadVoltage());
      double V_TP3=U(TP3.ReadVoltage());
      V[i]=GetU(V_TP1, V_TP3);
      delay(10);
      sum += V[i];
    }
    double mean = sum / 100.0;
    double variance = 0.0;
    for (i = 0; i < 100; i++){
      variance += pow(V[i] - mean, 2);
    }
    variance /= 100;
    if (variance >= 0.001) {
      Serial.println("This is a C");
      Serial.print("C = ");
      Serial.print(GetC(TP1, TP3));
      Serial.println(" uF");
    }
    else {
      Serial.println("This is a R");
      Serial.print("R = ");
      Serial.print(GetR(TP1, TP3));    
      Serial.print(" ou");    
    }
  }

  void decide_element(TestPoint TP1, TestPoint TP2, TestPoint TP3){
    bool PNP=isPNP(TP1, TP2, TP3);
    bool NPN=isNPN(TP1, TP2, TP3);
    if (PNP){
      Serial.println("This is a PNP");
      GetPNP(TP1, TP2, TP3);
    }
    else if (NPN){
      Serial.println("This is a NPN");
      GetNPN(TP1, TP2, TP3);
    }
    else{
      isCorR(TP1, TP2, TP3);
    }
  }

  void describGraph(TestPoint TP1, TestPoint TP2, TestPoint TP3){
    int list[11] = {50,70,90,110,130,150,170,190,210,230,250};
    TP1.SetState(1);
    TP3.SetState(2);
    double Vb, Ve, Re, Rc, Rb, Ib, Ie, beta, Ube;
    for (int i = 0; i < 11; i++){
      TP2.SetStateDigital(list[i]);
      Vb=U(TP2.ReadVoltage());
      Serial.println(Vb);
      Ve=U(TP3.ReadVoltage());
      Ube = GetU(Vb, Ve);
      Rc = 1000.0;
      Re = 1000.0;
      Rb = 680000.0;
      Ib = (5-Vb)/Rb;
      Ie = Ve/Re;
      beta = Ie/Ib;
      Serial.print("Ib:");
      Serial.print(1000.0*Ib,3);
      Serial.print("mA  ");
      Serial.print("Ie:");
      Serial.print(1000.0*Ie,3);
      Serial.print("mA  ");
      Serial.print("Ube:");
      Serial.print(Ube,6);
      Serial.print("V  ");
      Serial.print("beta:");
      Serial.println(beta,3);
    }
  }

  void loop() {
    // //电阻
    // Serial.print("Rx: ");
    // Serial.println(GetR(TP1,TP2));
    // Serial.println(TP1.ReadVoltage());
    // Serial.println(TP2.ReadVoltage());
    // Serial.println(TP3.ReadVoltage());

    // Serial.print(GetR(TP1,TP2));
    // Serial.println(" ou");

    // //电容
    // Serial.print("Cx: ");
    // Serial.print(GetC(TP1, TP3));
    // Serial.println(" uF");
    // Serial.print("Is it NPN? ");
    // Serial.println(isNPN(TP1, TP2, TP3));
    // Serial.print("Is it PNP? ");
    // Serial.println(isPNP(TP1, TP2, TP3));
    // describGraph(TP1, TP2, TP3);
    // int list[11] = {50,70,90,110,130,150,170,190,210,230,250};
    // TP1.SetState(1);
    // TP3.SetState(2);
    // double Vb, Ve, Re, Rc, Rb, Ib, Ie, beta, Ube;
    // for (int i = 0; i < 11; i++){
    //   TP2.SetStateDigital(list[i]);
    //   Vb=U(TP2.ReadVoltage());
    //   Serial.println(Vb);
    // }    
    // TP2.SetStateDigital(150);
    // Vb=U(TP2.ReadVoltage());
    // Serial.println(Vb);
    // GetPNP(TP1, TP2, TP3);
    // isCorR(TP1, TP2, TP3);
    // decide_element(TP1, TP2, TP3);
    // return;
    Serial.println("This is a C");
    Serial.print("C = ");
    Serial.print(GetC(TP1, TP3));
    Serial.println(" uF");
    // Serial.println("This is a R");
    // Serial.print("R = ");
    // Serial.println(GetR(TP1, TP3));
    // Serial.println("This is a L");
    // Serial.print("L = ");
    // Serial.print(GetL(TP1, TP3, 100000));
    // Serial.println(" H");
    // isCorR(TP1, TP2, TP3);
    // Serial.println("This is a R");
    // Serial.print("R = ");
    // Serial.print(GetR(TP1, TP3));    
    // Serial.println(" ou");    

    delay(2000); // 延迟1秒，避免串口输出过于频繁
  }
