//oled

//oled

void displayLoadingAnimation() {
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    int radius = 10;
    int angle = 0;

    for (int i = 0; i < 12; i++) { // Loop untuk animasi loading
        display.clearDisplay();

        // Gambar lingkaran dasar
        display.drawCircle(centerX, centerY, radius, SSD1306_WHITE);

        // Gambar titik animasi berdasarkan sudut
        float radian = (angle * 3.14) / 180.0;
        int dotX = centerX + radius * cos(radian);
        int dotY = centerY + radius * sin(radian);
        display.fillCircle(dotX, dotY, 2, SSD1306_WHITE);

        // Tampilkan teks loading
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(centerX - 30, centerY + 15);
        display.println(F("Loading..."));

        display.display();
        angle += 30; // Naikkan sudut 30 derajat
        if (angle >= 360) angle = 0; // Reset jika sudut lebih dari 360 derajat
        delay(150); // Kecepatan animasi
    }
}


void displayConnectedInfo() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Connected to"));
  display.println(F("Modbus Server"));
  display.println(F("  "));
  display.print(F("IP: "));
  display.println(WiFi.localIP());
  display.display();
  delay(5000);
}

void clockOLED() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);

  String currentTime = getTimeString();
  display.println(currentTime);
  display.display();
}
