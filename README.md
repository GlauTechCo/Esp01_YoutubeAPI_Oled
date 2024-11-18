AMS1117 modülü ve 4 pin Type-C soketini ekleyecek şekilde metni revize ettim:  

---

# ESP-01 ile YouTube Kanal İstatistiklerini Gösterme  

Bu proje, ESP-01 mikrodenetleyicisi ve bir OLED ekran kullanarak bir YouTube kanalının abone sayısını ve toplam görüntülenme sayısını gösterir. Veriler, YouTube Data API'si kullanılarak gerçek zamanlı olarak alınır.  

## Özellikler  

- ESP-01 üzerinden Wi-Fi bağlantısı  
- YouTube Data API entegrasyonu  
- OLED ekran üzerinde abone ve görüntülenme sayısının dönüşümlü olarak gösterilmesi  
- Her 10 dakikada bir verilerin otomatik güncellenmesi  

## Donanım Gereksinimleri  

- **ESP-01**  
- **OLED Ekran** (128x32 çözünürlük, I2C arayüzü)  
- **AMS1117 3.3V Regülatör Modülü** (ESP-01 ve OLED ekran için kararlı bir 3.3V güç kaynağı sağlar)  
- **4 Pin Type-C Soketi** (Harici güç ve veri bağlantısı için kullanılabilir)  
- **USB UART Dönüştürücü** (ESP-01'i programlamak için)  
- I2C bağlantısı:  
  - `SCL` → **GPIO0** (ESP-01 için manuel olarak bağlanmalı)  
  - `SDA` → **GPIO2** (ESP-01 için manuel olarak bağlanmalı)  

### Ek Bağlantılar  
- **AMS1117 Modülü**:  
  - **Giriş (IN)** → 5V (Type-C soketinden alınabilir)  
  - **Çıkış (OUT)** → ESP-01'in ve OLED ekranın 3.3V pinine bağlanır  
  - **GND** → Ortak toprak hattına bağlanır  
- **Type-C Soketi**:  
  - **VCC (5V)** → AMS1117 modülünün girişine bağlanır  
  - **GND** → Ortak toprak hattına bağlanır  

## Yazılım Gereksinimleri  

Aşağıdaki Arduino kütüphanelerinin kurulması gerekir:  

- [ESP8266WiFi](https://arduino-esp8266.readthedocs.io/)  
- [WiFiClientSecure](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)  
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)  
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)  
- [YoutubeApi](https://github.com/witnessmenow/arduino-youtube-api)  
- [ArduinoJson](https://arduinojson.org/)  

## Kurulum  

1. **Donanım Bağlantıları**:  
   - ESP-01'inizi bir USB UART dönüştürücüye bağlayarak programlayın.  
   - OLED ekranın pinlerini ESP-01'in GPIO pinlerine bağlayın:  
     - **SDA** → GPIO2  
     - **SCL** → GPIO0  
   - AMS1117 modülü ile Type-C soketi arasındaki bağlantıları yapın:  
     - Type-C soketinin 5V çıkışı AMS1117'nin girişine bağlanır.  
     - AMS1117'nin 3.3V çıkışı ESP-01 ve OLED ekrana güç sağlar.  
   - 3.3V güç kaynağı kullandığınızdan emin olun; ESP-01 ve OLED ekran 3.3V ile çalışır.  

2. **Arduino IDE Ayarları**:  
   - Araçlar > Kart > "Generic ESP8266 Module" seçin.  
   - Araçlar > Flash Mode > "DIO" seçeneğini seçin.  
   - Doğru bağlantı noktasını (COM port) seçtiğinizden emin olun.  

3. **YouTube API Bilgilerini Alın**:  
   - [Google Cloud Console](https://console.cloud.google.com/) sayfasına gidin ve oturum açın.  
   - **Yeni Proje Oluştur**:  
     - "Proje oluştur" düğmesine tıklayın ve bir proje adı girin.  
   - **API'yi etkinleştir**:  
     - Soldaki menüden **API ve Hizmetler > Kitaplık** sekmesine gidin.  
     - **YouTube Data API v3**'ü bulun ve "Etkinleştir" düğmesine tıklayın.  
   - **API Anahtarı oluştur**:  
     - **Kimlik Bilgileri** sekmesine gidin ve "Kimlik bilgileri oluştur" düğmesine tıklayın.  
     - "API Anahtarı" seçeneğini seçin. Oluşturulan API anahtarını kaydedin.  
   - **Kanal Kimliğini Bulma**:  
     - YouTube'da kanalınıza gidin.  
     - URL'deki `https://www.youtube.com/channel/` kısmından sonraki kod sizin **Kanal ID**'nizdir. Örneğin: `UCVmo1EnyRwKl4Y478WpDT1w`.  

4. **Kodu Düzenleyin**:  
   - `ssid` ve `password` değişkenlerini kendi Wi-Fi bilgilerinizle değiştirin.  
   - `YT_DATA_API_KEY` ve `YT_CHANNEL_ID` değerlerini aldığınız API anahtarı ve Kanal ID ile değiştirin.  

5. **Kütüphaneleri Yükleyin**:  
   - Arduino IDE'de **Araçlar > Kütüphane Yöneticisi**'ni açın ve yukarıda belirtilen kütüphaneleri yükleyin.  

6. **Kodu Yükleyin**:  
   - Arduino IDE üzerinden kodu ESP-01'e yükleyin.  
   - Seri monitörü açarak bağlantı ve güncelleme durumunu kontrol edin.  

## Çalışma  

- Cihaz bağlandıktan sonra, OLED ekranında kanal istatistikleri gösterilecektir.  
- Ekran, her 5 saniyede bir abone ve görüntülenme sayısını dönüşümlü olarak gösterir.  
- Her 10 dakikada bir veriler YouTube API'si üzerinden güncellenir.  

## Görüntü Örneği  

_OLED ekran üzerinde kanal logonuz ve istatistikler şu şekilde görünür:_  
Logo  
Takipçi Sayısı: 12345  
Logo  
Görüntüleme Sayısı: 67890  
