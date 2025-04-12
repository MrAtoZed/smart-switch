//html

void handleWebPage() {
  timeClient.update();
  String page = "<html><head><title>ESP8266 Clock</title>";

  // Script untuk update jam setiap detik
  page += "<script>setInterval(()=>{fetch('/time').then(r=>r.text()).then(t=>document.getElementById('clock').innerHTML=t)},1000);</script>";

  // Gaya CSS untuk tata letak horizontal
  page += "<style>";
  page += "body { background-color: #1E3A8A; color: white; font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 20px; }";
  page += "h1 { color: #60A5FA; font-size: 28px; }";
  page += "#clock { background: #60A5FA; padding: 10px; display: inline-block; font-size: 24px; border-radius: 8px; }";
  page += ".container { display: flex; justify-content: center; gap: 20px; margin-top: 20px; }";
  page += ".coil-box { background: #2563EB; padding: 15px; border-radius: 10px; min-width: 200px; max-width: 250px; }";
  page += "ul { list-style: none; padding: 0; margin: 0; display: flex; flex-direction: column; min-height: 50px; max-height: 300px; overflow-y: auto; }";
  page += "li { background: #3B82F6; margin: 5px 0; padding: 10px; border-radius: 5px; }";
  page += "a { color: red; text-decoration: none; font-weight: bold; }";
  page += "a:hover { color: darkred; }";
  page += "form { background: #2563EB; padding: 15px; border-radius: 10px; display: inline-block; margin-top: 20px; }";
  page += "form input, select { padding: 5px; margin: 5px; border-radius: 5px; border: none; }";
  page += "input[type='submit'] { background: #60A5FA; color: white; padding: 10px 20px; cursor: pointer; border: none; border-radius: 5px; font-weight: bold; }";
  page += "input[type='submit']:hover { background: #3B82F6; }";
  page += "</style>";

  // Mulai Body
  page += "</head><body>";
  page += "<h1>ESP8266 Web Scheduling</h1>";
  page += "<h2 id='clock'>" + getTimeString() + "</h2>";

  // Container untuk menampilkan jadwal secara horizontal
  page += "<div class='container'>";
  for (int coil = 1; coil <= 4; coil++) {
    page += "<div class='coil-box'>";
    page += "<h3>Jadwal Coil " + String(coil) + ":</h3><ul>";
    
    std::vector<Schedule>* schedules;
    switch (coil) {
      case 1: schedules = &schedules1; break;
      case 2: schedules = &schedules2; break;
      case 3: schedules = &schedules3; break;
      case 4: schedules = &schedules4; break;
    }

    for (size_t i = 0; i < schedules->size(); i++) {
      page += "<li>" + String((*schedules)[i].startHour) + ":" + String((*schedules)[i].startMinute) +
              " - " + String((*schedules)[i].endHour) + ":" + String((*schedules)[i].endMinute) +
              " <a href='delete_time?coil=" + String(coil) + "&id=" + String(i) + "'>[Hapus]</a></li>";
    }
    page += "</ul></div>";
  }
  page += "</div>";  // Akhir container

  // Form untuk menambahkan jadwal baru
  page += "<h3>Tambah Jadwal:</h3>";
  page += "<form action='/set_time' method='POST'>";
  page += "Coil: <select name='coil'>";
  for (int i = 1; i <= 4; i++) {
    page += "<option value='" + String(i) + "'>Coil " + String(i) + "</option>";
  }
  page += "</select><br>";
  page += "Start Hour: <input type='number' name='startHour' min='0' max='23' required> : ";
  page += "<input type='number' name='startMinute' min='0' max='59' required><br>";
  page += "End Hour: <input type='number' name='endHour' min='0' max='23' required> : ";
  page += "<input type='number' name='endMinute' min='0' max='59' required><br>";
  page += "<br><input type='submit' value='Set Time'>";
  page += "</form>";

  page += "</body></html>";

  server.send(200, "text/html", page);
}

/*void handleWebPage() {
  timeClient.update();
  String page = "<html><head><title>ESP8266 Clock</title>";
  page += "<script>setInterval(()=>{fetch('/time').then(r=>r.text()).then(t=>document.getElementById('clock').innerHTML=t)},1000);</script>";
  page += "<style>body{text-align:center;font-family:Arial;margin-top:50px;}h1{color:#333;}form{margin-top:20px;}</style>";
  page += "</head><body><h1>ESP8266 Web Clock</h1>";
  page += "<h2 id='clock'>" + getTimeString() + "</h2>";

  // Menampilkan jadwal untuk masing-masing coil
  for (int coil = 1; coil <= 4; coil++) {
    page += "<h3>Jadwal Coil " + String(coil) + ":</h3><ul>";
    std::vector<Schedule>* schedules;
    switch (coil) {
      case 1: schedules = &schedules1; break;
      case 2: schedules = &schedules2; break;
      case 3: schedules = &schedules3; break;
      case 4: schedules = &schedules4; break;
    }
    for (size_t i = 0; i < schedules->size(); i++) {
      page += "<li>" + String((*schedules)[i].startHour) + ":" + String((*schedules)[i].startMinute) +
              " - " + String((*schedules)[i].endHour) + ":" + String((*schedules)[i].endMinute) +
              " <a href='delete_time?coil=" + String(coil) + "&id=" + String(i) + "' style='color:red;'>[Hapus]</a></li>";
    }
    page += "</ul>";
  }

  // Form untuk menambahkan jadwal baru
  page += "<h3>Tambah Jadwal:</h3>";
  page += "<form action='/set_time' method='POST'>";
  page += "Coil: <select name='coil'>";
  for (int i = 1; i <= 4; i++) {
    page += "<option value='" + String(i) + "'>Coil " + String(i) + "</option>";
  }
  page += "</select><br>";
  page += "Start Hour: <input type='number' name='startHour' min='0' max='23' required> : ";
  page += "<input type='number' name='startMinute' min='0' max='59' required><br>";
  page += "End Hour: <input type='number' name='endHour' min='0' max='23' required> : ";
  page += "<input type='number' name='endMinute' min='0' max='59' required><br>";
  page += "<br><input type='submit' value='Set Time'>";
  page += "</form>";

  page += "</body></html>";

  server.send(200, "text/html", page);
}*/