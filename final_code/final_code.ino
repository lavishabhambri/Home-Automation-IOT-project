#include <WiFi.h>
#include <WebServer.h>
#include "ThingSpeak.h"
#include "ESP32_MailClient.h" // esp-mail-client 
#include "DHT.h"
#include "HTTPClient.h"
#include "time.h"
#include <ArduinoJson.h>
#define GMAIL_SMTP_SEVER "smtp.gmail.com"
#define GMAIL_SMTP_USERNAME "abc@gmail.com" // gmail
#define GMAIL_SMTP_PASSWORD "your_password" // pwd
#define GMAIL_SMTP_PORT 465


#define PIR 2
#define BUZZER 4
#define LDR 32
#define LED_LDR 19
#define DHTTYPE DHT11
#define dht_dpin 27
#define LED 26

float threshold = 25.00; // We found it to be 55-59 with candle.
String Fire_alarm_status = "";
String Motion_sensor_status = "";
// Initialize DHT sensor.
DHT dht(dht_dpin, DHTTYPE);
const char *ssid = "wifi_ssid";
const char *password = "wifi_pasword";

//Things Speak
unsigned long myChannelNumber = your_channel_number;
const char * myWriteAPIKey = "your_api_key";
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
WiFiClient  client;


WebServer server(80);
// EMail Data
SMTPData data;
char* fire_mail = "<!DOCTYPE html><html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:o=\"urn:schemas-microsoft-com:office:office\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"> <meta name=\"x-apple-disable-message-reformatting\"> <title></title> <!--[if mso]><noscript><xml><o:OfficeDocumentSettings><o:PixelsPerInch>96</o:PixelsPerInch></o:OfficeDocumentSettings></xml></noscript><![endif]--> <style> table, td, div, h1, p { font-family: Arial, sans-serif; } </style></head><body style=\"margin:0;padding:0;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;background:#ffffff;\"> <tr> <td align=\"center\" style=\"padding:0;\"> <table role=\"presentation\" style=\"width:602px;border-collapse:collapse;border:1px solid #cccccc;border-spacing:0;text-align:left;\"> <tr> <td align=\"center\" style=\"padding:20px 0 10px 0;background:#70bbd9;\"> <img src=\"https://i.ibb.co/x8X4KFf/Pin-Clipart-com-jail-clipart-12844.png\" width=\"150\" style=\"height:auto;display:block;\" /> </td> </tr> <tr> <td align=\"center\" style=\"background:#70bbd9;\"> <p style=\"font-size: 3rem;\"> Security Alert!</p> </td> </tr> <tr> <td style=\"padding:36px 30px 42px 30px;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;\"> <tr> <td style=\"padding:0 0 0 0;color:#153643;\"> <h1 style=\"font-size:24px;margin:0 0 20px 0;font-family:Arial,sans-serif;\"> Emergency!!</h1> <p style=\"margin:0 0 12px 0;font-size:1.75rem;line-height:24px;font-family:Arial,sans-serif;color:red;\"> Fire detected at your house. Hurry!</p> </td> </tr> <tr> <td align=\"center\"> <img src=\" https://i.ibb.co/GWsHYP4/Pngtree-burning-fire-vector-element-5816475.png\" width=\"200\" style=\"height:auto;display:block;\" /> </td> </tr> </table> </td> </tr> <tr> <td style=\"padding:30px;background:#ee4c50;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;font-size:9px;font-family:Arial,sans-serif;\"> <tr> <td style=\"padding:0;width:50%;\" align=\"left\"> <p style=\"margin:0;font-size:14px;line-height:16px;font-family:Arial,sans-serif;color:#ffffff;\"> Team Electricians<br />IoT Project </p> </td> </tr> </table> </td> </tr> </table> </td> </tr> </table></body></html>";
char* motion_mail = "<!DOCTYPE html><html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:o=\"urn:schemas-microsoft-com:office:office\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"> <meta name=\"x-apple-disable-message-reformatting\"> <title></title> <!--[if mso]><noscript><xml><o:OfficeDocumentSettings><o:PixelsPerInch>96</o:PixelsPerInch></o:OfficeDocumentSettings></xml></noscript><![endif]--> <style> table, td, div, h1, p { font-family: Arial, sans-serif; } </style></head><body style=\"margin:0;padding:0;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;background:#ffffff;\"> <tr> <td align=\"center\" style=\"padding:0;\"> <table role=\"presentation\" style=\"width:602px;border-collapse:collapse;border:1px solid #cccccc;border-spacing:0;text-align:left;\"> <tr> <td align=\"center\" style=\"padding:20px 0 10px 0;background:#70bbd9;\"> <img src=\"https://i.ibb.co/x8X4KFf/Pin-Clipart-com-jail-clipart-12844.png\" width=\"150\" style=\"height:auto;display:block;\" /> </td> </tr> <tr> <td align=\"center\" style=\"background:#70bbd9;\"> <p style=\"font-size: 3rem;\"> Security Alert!</p> </td> </tr> <tr> <td style=\"padding:36px 30px 42px 30px;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;\"> <tr> <td style=\"padding:0 0 0 0;color:#153643;\"> <h1 style=\"font-size:24px;margin:0 0 20px 0;font-family:Arial,sans-serif;\"> Emergency!!</h1> <p style=\"margin:0 0 12px 0;font-size:1.75rem;line-height:24px;font-family:Arial,sans-serif;color:red;\"> Someone is inside your house. Hurry!</p> </td> </tr> <tr> <td align=\"center\" style=\"padding:20px 0 0\"> <img src=\" https://i.ibb.co/f1W92Z9/pngegg.png\" width=\"200\" style=\"height:auto;display:block;\" /> </td> </tr> </table> </td> </tr> <tr> <td style=\"padding:30px;background:#ee4c50;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;font-size:9px;font-family:Arial,sans-serif;\"> <tr> <td style=\"padding:0;width:50%;\" align=\"left\"> <p style=\"margin:0;font-size:14px;line-height:16px;font-family:Arial,sans-serif;color:#ffffff;\"> Team Electricians<br />IoT Project </p> </td> </tr> </table> </td> </tr> </table> </td> </tr> </table></body></html>";


String cse_ip = "192.168.1.9"; // YOUR IP from ipconfig/ifconfig

String cse_port = "8080";
String server_OM2M = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";

String ae1 = "DHT11";
String cnt1 = "TEMPERATURE";

String ae2 = "PIR";
String cnt2 = "MOTION";

String ae3 = "LED";
String cnt3 = "STATUS";

String ae4 = "BUZZER";
String cnt4 = "STATUS";

String ae5 = "LDR_LED";
String cnt5 = "STATUS";


void createCI(String& val, String ae, String cnt) {
  // add the lines in step 3-6 inside this function
  HTTPClient http;
  http.begin(server_OM2M + ae + "/" + cnt + "/");

  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");

  Serial.println(code);
  if (code == -1) {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

String sendEmail(char *subject, char *sender, char *body, char *recipient, boolean htmlFormat) {
  data.setLogin(GMAIL_SMTP_SEVER, GMAIL_SMTP_PORT, GMAIL_SMTP_USERNAME, GMAIL_SMTP_PASSWORD);
  data.setSender(sender, GMAIL_SMTP_USERNAME);
  data.setSubject(subject);
  data.setMessage(body, htmlFormat);
  data.addRecipient(recipient);
  if (!MailClient.sendMail(data))
    return MailClient.smtpErrorReason();

  return "";
}

bool motion_detected = 0;
bool fire_detected = 0;
String page = "";
int HOME_LED = 0;
int BUZZER_READING = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(PIR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LDR, INPUT);
  pinMode(LED_LDR, OUTPUT);
  pinMode(dht_dpin, INPUT);
  dht.begin();
  ThingSpeak.begin(client);
  delay(50);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  page = "<head><style>body {font-family: sans-serif;background-color: #98ddca;}h1 {text-align: center;color: white;font-size: 3.5em;}h2 {color: white;font-size: 2.5em;margin-bottom: 7%;}.card {background-color: #ffaaa7;border-radius: 10px;padding: 5%;margin: 20px 10%;}.button {background-color: #ffd3b4;color: black;border-radius: 10px;padding: 10px 12%;margin: 0 2% 0 0;text-decoration: none;}p {color: black;font-size: 1.3rem;}</style></head><h1>IoT Project</h1><div class=\"card\"><h2>Home Automation</h2><p><a class=\"button\" href=\"LEDOn\">ON</a><a class=\"button\" href=\"LEDOff\">OFF</a></p></div><br /><div class=\"card\"><h2>Home Security</h2><p id=\"mdata\"></p></div><br /><div class=\"card\"><h2>Fire Security</h2><p id=\"fdata\"></p></div><script>var mdat = setInterval(function () {loadValues_m(\"datam.txt\", mupdateValues);}, 1000);function loadValues_m(url, callback) {var xhttp = new XMLHttpRequest();xhttp.onreadystatechange = function () {if(this.readyState == 4 && this.status == 200){callback.apply(xhttp);}};xhttp.open(\"GET\", url, true);xhttp.send();}function mupdateValues() {document.getElementById(\"mdata\").innerHTML = this.responseText; setcolor();}var fdat = setInterval(function () {loadValues_f(\"dataf.txt\", fupdateValues);}, 1000);function loadValues_f(url, callback) {var xhttp = new XMLHttpRequest();xhttp.onreadystatechange = function () {if (this.readyState == 4 && this.status == 200) {callback.apply(xhttp);}};xhttp.open(\"GET\", url, true);xhttp.send();}function fupdateValues() {document.getElementById(\"fdata\").innerHTML = this.responseText; setcolor();}function setcolor() {var md = document.getElementById(\"mdata\");if (md.innerHTML == \"Everything is safe!\") {md.style.color = \"green\";} else {md.style.color = \"red\";}var fd = document.getElementById(\"fdata\");if (fd.innerHTML == \"Everything is safe!\") {fd.style.color = \"green\";}else{fd.style.color = \"red\";}}</script>";

  server.on("/", []()
  {
    server.send(200, "text/html", page);
  });
  server.on("/LEDOn", []()
  {
    server.send(200, "text/html", page);
    digitalWrite(LED, HIGH);
    HOME_LED = 1;
    delay(1000);
  });
  server.on("/LEDOff", []()
  {
    server.send(200, "text/html", page);
    digitalWrite(LED, LOW);
    HOME_LED = 0;
    delay(1000);
  });
  server.on("/dataf.txt", []()
  {
    if (fire_detected) Fire_alarm_status = "Come fast! Fire in your house";
    else Fire_alarm_status = "Everything is safe!";
    server.send(200, "text/html", Fire_alarm_status);
    // Reset Value
    Fire_alarm_status = "";
  });
  server.on("/datam.txt", []()
  {
    if (motion_detected) Motion_sensor_status = "Come fast! Someone is in your house";
    else Motion_sensor_status = "Everything is safe!";
    server.send(200, "text/html", Motion_sensor_status);
    // Reset Value
    Motion_sensor_status = "";
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    server.handleClient();
    motion_detected = digitalRead(PIR);                     // Taking readings from PIR sensor
    float Temperature = dht.readTemperature();              // Taking readings from DHT sensor
    float Humidity = dht.readHumidity();
    if (isnan(Temperature))
    {
      Temperature = 0;
    }
    if (isnan(Humidity))
    {
      Humidity = 0;
    }

    fire_detected = Temperature > threshold;
    if (motion_detected && fire_detected)                   // Turning Buzzer on if fire and motion is detected
    {
      digitalWrite(BUZZER, HIGH);
      BUZZER_READING = 1;
      String result1 = sendEmail("[IMPORTANT] Fire Detected!", "ESP32", fire_mail, "shreyashjain1007@gmail.com", true);    // Sending Mail if Fire
      String result2 = sendEmail("[IMPORTANT] Motion Detected", "ESP32", motion_mail, "shreyashjain1007@gmail.com", true); // Sending Mail if Motion
      Serial.println("Mail sent!");
      delay(5000);
    }
    else if (motion_detected)                   // Turning Buzzer on if motion is detected
    {
      digitalWrite(BUZZER, HIGH);
      BUZZER_READING = 1;
      String result2 = sendEmail("[IMPORTANT] Motion Detected", "ESP32", motion_mail, "shreyashjain1007@gmail.com", true); // Sending Mail if Motion
      delay(5000);
    }
    else if (fire_detected)                   // Turning Buzzer on if fire is detected
    {
      digitalWrite(BUZZER, HIGH);
      BUZZER_READING = 1;
      String result1 = sendEmail("[IMPORTANT] Fire Detected!", "ESP32", fire_mail, "shreyashjain1007@gmail.com", true);    // Sending Mail if Fire
      delay(5000);
    }
    else
    {
      digitalWrite(BUZZER, LOW);
      BUZZER_READING = 0;
    }


    int LDR_Reading = digitalRead(LDR);                     // Taking Readings from LDR sensor
    if (LDR_Reading == 0)                                   // Checking if LDR is receiving light, if not then turn the LED on.
    {
      digitalWrite(LED_LDR, LOW); // LED_LDR OFF
      Serial.println("LED_LDR OFF");
    }
    else
    {
      digitalWrite(LED_LDR, HIGH); // LED_LDR ON
      Serial.println("LED_LDR ON");
    }
    Serial.print("Motion: ");                               // Printing Values
    Serial.println(motion_detected);
    Serial.print("Temperature: ");
    Serial.println(Temperature);
    Serial.print("Humidity: ");
    Serial.println(Humidity);

    // Adding values to OM2M
    String val1 = String(Temperature, 3);
    createCI(val1, ae1, cnt1);

    String val2 = String(motion_detected);
    createCI(val2, ae2, cnt2);

    String val3 = String(HOME_LED);
    createCI(val3, ae3, cnt3);

    String val4 = String(BUZZER_READING);
    createCI(val4, ae4, cnt4);

    String val5 = String(LDR_Reading);
    createCI(val5, ae5, cnt5);

    // Adding DHT11 readings to Things Speak
    int x = ThingSpeak.writeField(myChannelNumber, 1, Temperature, myWriteAPIKey);

    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }

}
