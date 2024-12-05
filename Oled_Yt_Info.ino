#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <YoutubeApi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char ssid[] = "XXX";
char password[] = "XXX";

#define YT_DATA_API_KEY "XXX"
#define YT_CHANNEL_ID "XXX"

WiFiClientSecure client;
YoutubeApi api(YT_DATA_API_KEY, client);

unsigned long previousMillis = 0;
unsigned long displayDuration = 5000; // Her gösterim süresi 5 saniye
unsigned long stageDuration = 5000;   // Aşamaların süresi 5 saniye
unsigned long apiInterval = 30 * 60 * 1000; // 30 dakika (30 * 60 * 1000 ms)
unsigned long lastApiCallTime = 0;  // Son API çağrısının zamanı
int stage = 0;                      // Gösterim aşaması (0: logo, 1: abone, 2: izlenme, 3: video sayısı)
unsigned long stageStartTime = 0;   // Aşamaların başladığı zaman

const unsigned char myBitmap[] PROGMEM = {
  // YouTube logosunun bitmap verisi burada olacak.
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xfe,
  0x7f, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff,
  0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xf8, 0x07, 0xff,
  0xff, 0xf8, 0x0f, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xfe,
  0x7f, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  Serial.begin(115200);
  Wire.begin(0, 2);
  client.setInsecure();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;)
      ; // SSD1306 başlatılamazsa sonsuza kadar bekle
  }

  display.clearDisplay();
  display.display();

  display.setCursor(0, 0);
  display.setTextColor(WHITE);

  display.print("Connecting WiFi: ");
  display.print(ssid);
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    display.print(".");
    display.display();
    delay(500);
  }

  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("WiFi connected.");
  display.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  display.println(ip);
  display.display();

  // İlk veri çekme
  fetchDataFromApi();

  stageStartTime = millis(); // İlk aşamanın başlangıç zamanını ayarla
}

void fetchDataFromApi() {
  if (api.getChannelStatistics(YT_CHANNEL_ID)) {
    // API'den veri başarılı bir şekilde alındığında debug mesajı kaldırıldı
  } else {
    // API'den veri alınamadığında herhangi bir işlem yapılmaz
  }
  lastApiCallTime = millis(); // API çağrısının zamanını güncelle
}

int getTextWidth(String text) {
  return text.length() * 6; // SSD1306 font uzunluğunun 6 piksel olduğunu varsayıyoruz
}

void loop() {
  unsigned long currentMillis = millis();

  // 30 dakikada bir API'den veri çekmek için zaman kontrolü
  if (currentMillis - lastApiCallTime >= apiInterval) {
    fetchDataFromApi(); // API'den veri çek
  }

  if (currentMillis - stageStartTime >= stageDuration) {
    // 5 saniye tamamlandığında, aşamayı değiştir
    stageStartTime = currentMillis; // Aşamayı sıfırla

    if (stage == 0) {
      // Logo gösterimi
      stage = 1; // Logo sonrası abone sayısına geç
    } else if (stage == 1) {
      // Abone sayısı gösterimi
      stage = 2; // Abone sayısı sonrası izlenme sayısına geç
    } else if (stage == 2) {
      // İzlenme sayısı gösterimi
      stage = 3; // İzlenme sayısı sonrası video sayısına geç
    } else if (stage == 3) {
      // Video sayısı gösterimi
      stage = 1; // Video sayısı sonrası abone sayısına geri dön
    }
  }

  display.clearDisplay(); // Her seferinde ekranı temizle

  if (stage == 0) {
    // Logo gösterimi
    int xPos = (SCREEN_WIDTH - 32) / 2; // Bitmap'in yatayda ortalanacağı x konumu
    int yPos = (SCREEN_HEIGHT - 32) / 2; // Bitmap'in dikeyde ortalanacağı y konumu
    display.drawBitmap(xPos, yPos, myBitmap, 32, 32, WHITE);
    display.display();
  } else if (stage == 1) {
    // Abone sayısını göster
    String subscriberCount = String(api.channelStats.subscriberCount);
    String title = "ABONE SAYISI";

    int titleWidth = getTextWidth(title);
    int valueWidth = getTextWidth(subscriberCount);

    int titleXPos = (SCREEN_WIDTH - titleWidth) / 2;
    int valueXPos = (SCREEN_WIDTH - valueWidth) / 2;

    display.setCursor(titleXPos, 10);
    display.println(title);
    display.setCursor(valueXPos, 20);
    display.println(subscriberCount);
    display.display();
  } else if (stage == 2) {
    // İzlenme sayısını göster
    String viewCount = String(api.channelStats.viewCount);
    String title = "IZLENME SAYISI";

    int titleWidth = getTextWidth(title);
    int valueWidth = getTextWidth(viewCount);

    int titleXPos = (SCREEN_WIDTH - titleWidth) / 2;
    int valueXPos = (SCREEN_WIDTH - valueWidth) / 2;

    display.setCursor(titleXPos, 10);
    display.println(title);
    display.setCursor(valueXPos, 20);
    display.println(viewCount);
    display.display();
  } else if (stage == 3) {
    // Video sayısını göster
    String videoCount = String(api.channelStats.videoCount);
    String title = "VIDEO SAYISI";

    int titleWidth = getTextWidth(title);
    int valueWidth = getTextWidth(videoCount);

    int titleXPos = (SCREEN_WIDTH - titleWidth) / 2;
    int valueXPos = (SCREEN_WIDTH - valueWidth) / 2;

    display.setCursor(titleXPos, 10);
    display.println(title);
    display.setCursor(valueXPos, 20);
    display.println(videoCount);
    display.display();
  }

  delay(100); // Fazla yüklenmeyi önlemek için kısa bir gecikme
}
