String criar_pagina(int qtd_sensores, int * ptr) {
  String pagina = "<table><tr>";
  for (int i = 1; i <= qtd_sensores; i++) {
    pagina += "<td>Sensor  " + String(i) + "</td>";
  }
  pagina += "</tr><tr>";

  for (int i = 0; i < max_data_bpm; i++) {
    for (int j = 0; j < qtd_sensores; j++) {
      pagina += "<td>" + (String)*(ptr+i+(j)*max_data_bpm) + "</td>";
    }
    pagina += "</tr>";
  }
  //Serial.println(*ptr);
  pagina += "</table>";
  return pagina;
}

void handle_OnConnect() {
  String page = "";
  page = "<h1>Sensor to Node MCU Web Server</h1><h1>BPM:</h1> <h1 id=\"data\">""</h1>\r\n";
  page += "<script>\r\n";
  page += "var x = setInterval(function() {loadData(\"data.txt\",updateData)}, 1000);\r\n";
  page += "function loadData(url, callback){\r\n";
  page += "var xhttp = new XMLHttpRequest();\r\n";
  page += "xhttp.onreadystatechange = function(){\r\n";
  page += " if(this.readyState == 4 && this.status == 200){\r\n";
  page += " callback.apply(xhttp);\r\n";
  page += " }\r\n";
  page += "};\r\n";
  page += "xhttp.open(\"GET\", url, true);\r\n";
  page += "xhttp.send();\r\n";
  page += "}\r\n";
  page += "function updateData(){\r\n";
  page += " document.getElementById(\"data\").innerHTML = this.responseText;\r\n";
  page += "}\r\n";
  page += "</script>\r\n";
  server.send(200, "text/html", page);
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
