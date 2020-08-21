#include <LiquidCrystal.h>
#include <Keypad.h>
#include <TimeLib.h>

// KAYPADIN KARAKTER DUZENI BELIRLENDI
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'-', '0', 'O', 'D'}
};
byte ypins[4] = {5, 4, 3, 2};
byte xpins[4] = {9, 8, 7, 6};
String kayitlar[20] ;
int dizi_indis = 0;
int dizi_doluluk = 0;

String lcd_deger, terminal_deger, sifre = "00000000";
boolean keypad_giris = false, guvenlik_sistemi = true, com_giris = false;
char key;


LiquidCrystal lcd(20, 21, 13, 12, 11, 10);
Keypad keypad = Keypad (makeKeymap(keys), xpins, ypins, 4, 4);

void setup() {
  // IHTIYAC DUYDUGUM PINLERI CIKIS OLARAK AYARLADIM
  for (int x = 2; x < 28; x++)
    pinMode(x, OUTPUT);


  lcd.begin(16, 2);
  Serial.begin(9600);
  setTime(12, 59, 11, 12, 9, 2017);
  delay(5);
  lcd.setCursor(0, 0);
  lcd.print("SIFRE GIRISI:");
  lcd.setCursor(0, 1);
  Serial.println("Sisteme giris saati: " + getTarihSaat());
  SicaklikDegeriniYaz();
  Kayitlar();
  Serial.print("Sifre girisi:");

}

void loop() {
  // COMDAN GIRIS YAPILMISSA ONA GORE ISLEMLER YAPACAK SEKILDE AYARLADIM
  if (com_giris) {
    if (Serial.available()) {
      terminal_deger = DegerAl();
      if (KomutIsle(terminal_deger))
        Serial.println(terminal_deger + " komutu islendi.");
      else
        Serial.println(terminal_deger + " komutu tanimlanamadi.");
    }
  }
  else {
    if (Serial.available()) {
      terminal_deger = DegerAl();
      if ( terminal_deger == sifre )
        Serial.println("Sisteme giris yapildi .!");
      com_giris = true;
      KomutBilgisi();
    }
  }

  // GUVENLIK SISTEMI AKTIFKEN SUREKLI OLARAK GUVENLIK KONTROLU SAGLAYACAK SEKILDE SISTEMIMI AYARLADIM
  if (guvenlik_sistemi) {
    if ( GuvenlikKontrol()) {

      digitalWrite(19, HIGH);
      delay(1000);
    }
    else
      digitalWrite(19, LOW);
  }


  key = keypad.getKey();

  if (key) //Eğer tusa basılmıssa işlem yapacak
  {
    if (keypad_giris) {
      OzelKarakter(key);
    }
    else if (lcdDeger() == sifre)
    {
      lcdTemizle();
      lcd.print("SISTEME GIRILDI");

      kayitlar[dizi_indis] = getTarihSaat();
      dizi_indis++;
      dizi_doluluk++;

      Hosgeldiniz();
      keypad_giris = true;

    }
    else {
      lcdTemizle();
      lcd.print("YANLIS SIFRE !");
      delay(1000);
      lcdTemizle();
      lcd.print("SIFRE GIRISI:");
      lcd.setCursor(0, 1);
    }
  }
}
void lcdTemizle() {
  lcd.clear();
  delay(5);
  lcd.setCursor(0, 0);
}

String lcdDeger() {
    // LCD GIRLEN KARAKTERLERI OKUMA ISLEMI GERCEKLESTIRILDI
  lcd_deger = "";
  while (true) {
    if (key)
    {
      if ((key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9') && (lcd_deger.length() < 8) ) {
        lcd.print('*');
        lcd_deger += key;
      }

      else if ( key == '-') {
        lcd_deger = lcd_deger.substring(0, lcd_deger.length() - 1);
        lcd.setCursor(lcd_deger.length(), 1);
        lcd.write(" ");
        lcd.setCursor(lcd_deger.length(), 1);
      }
      else if ( key == 'O') {

        Serial.println(lcd_deger);
        return lcd_deger;
      }
    }
    key = keypad.getKey();
  }
}
void Hosgeldiniz() {                                 // TARIH SAATI SISTEMDE IHTIYAC DUYDUGUM HERHANGI BIR NOKTADA ULASMAK ICIN METOD YAZDIM
  delay(1000);
  lcdTemizle();
  lcd.print("HOSGELDINIZ");

}
String getTarihSaat() {                              // TARIH SAATI SISTEMDE IHTIYAC DUYDUGUM HERHANGI BIR NOKTADA ULASMAK ICIN METOD YAZDIM
  return String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + ":" + String(minute()) + ":" + String(second());
}
int Sicaklik(int pin) {                             // GELEN PİN DEĞERİ OKUNUYOR
  pin = analogRead(pin);
  pin = map(pin, 0, 1023, 0, 180);                 // OKUNDAN DEGER 0-100 ARASINDA TUTULUYOR
  return pin;
}

void SicaklikDegeriniYaz() {
  Serial.print("ON oda sicaklik: ");
  Serial.print(Sicaklik(A0));
  Serial.println(" C");
  Serial.print("ARKA oda sicaklik: ");
  Serial.print(Sicaklik(A1));
  Serial.println(" C");
}
void Kayitlar() {
  for (int i = 0; i <= dizi_doluluk; i++)
    Serial.println(kayitlar[i]);
}

void OzelKarakter(char key) {
    // ALINAN KARAKTER BIR NUMARA DEGILSE OZEL BIR TUSSA ONA GORE ISLEM YAPILMASI AMACLANDI
  switch (key) {

    case 'A':
      lcdTemizle();
      lcd.print("GUVENLIK");
      lcd.setCursor(0, 1);
      lcd.print("AKTIF");
      guvenlik_sistemi = true;
      Hosgeldiniz();
      break;

    case 'B':
      lcdTemizle();
      lcd.print("GUVENLIK");
      lcd.setCursor(0, 1);
      lcd.print("PASIF");
      guvenlik_sistemi = false;
      Hosgeldiniz();
      break;

    case 'C':
      lcdTemizle();
      lcd.print("YENI SIFRE:");
      lcd.setCursor(0, 1);

      sifre = lcdDeger();
      delay(1000);
      lcdTemizle();
      lcd.print("SIFRE DEGISTI");
      Serial.println(sifre);
      Hosgeldiniz();
      break;

    case 'D':
      lcdTemizle();
      lcd.print("SISTEM CIKISI");
      delay(1000);
      lcdTemizle();
      lcd.print("SIFRE GIRISI:");
      lcd.setCursor(0, 1);
      keypad_giris = false;
      break;

  }
}
void KomutBilgisi() {
  
  // KULLANICI ISTERSE ERISEBILECEGI KOMUT BILGILERINI GONDEREN METHOD YAZILDI
  Serial.println("---->KOMUTLAR");
  Serial.println("elektrikac: Evdeki elektrik sistemini aktif eder.");
  Serial.println("elektrikkapat: Evdeki elektrik sistemini aktif eder.");
  Serial.println("suac: Evdeki su sistemini aktif eder.");
  Serial.println("sukapat: Evdeki su sistemini pasif eder.");
  Serial.println("dogalgazac: Evdeki dogalgaz sistemini aktif eder.");
  Serial.println("dogalgazkapat: Evdeki dogalgaz sistemini pasif eder.");
  Serial.println("onklimaac: On odadaki klimayi aktif eder.");
  Serial.println("onklimakapat: Arka odadaki klimayi pasif eder.");
  Serial.println("arkaklimaac: Arka odadaki klimayi aktif eder.");
  Serial.println("arkaklimakapat: Arka odadaki klimayi pasif eder.");
  Serial.println("sifredegis: Sifre degistirmek icin yeni sifre istenir.");
  Serial.println("tarihayarla: Sifre degistirmek icin yeni sifre istenir.");
  Serial.println("evsicaklik: Evdeki odalarin sicaklik bilgisini verir.");
  Serial.println("guvenlikaktif: Ev guvenligini aktif eder.");
  Serial.println("guvenlikpasif: Ev guvenligini pasif eder.");
  Serial.println("sistemzamani: Sistem zaman bilgisini verir.");
  Serial.println("sistemcikisi: Sistemden cikisi gerceklesir.");
  Serial.println("komutbilgisi: Sisteme tanimli komutlar hakkinda bilgi verir.");
}

boolean KomutIsle(String komut) {
  // ALINAN KOMUT BELİRLENEN KRİTERLERE GORE ISLENDI 
  boolean islem = true;
  if (terminal_deger == "elektrikac") digitalWrite(14, HIGH);
  else if (terminal_deger == "elektrikkapat") digitalWrite(14, LOW);
  else if (terminal_deger == "suac")digitalWrite(15, HIGH);
  else if (terminal_deger == "sukapat")digitalWrite(15, LOW);
  else if (terminal_deger == "dogalgazac")digitalWrite(16, HIGH);
  else if (terminal_deger == "dogalgazkapat")digitalWrite(16, LOW);
  else if (terminal_deger == "onklimaac")digitalWrite(17, HIGH);
  else if (terminal_deger == "onklimakapat")digitalWrite(17, LOW);
  else if (terminal_deger == "arkaklimaac")digitalWrite(18, HIGH);
  else if (terminal_deger == "arkaklimakapat")digitalWrite(18, LOW);
  else if (terminal_deger == "evsicaklik")SicaklikDegeriniYaz();
  else if (terminal_deger == "komutbilgisi")KomutBilgisi();
  else if (terminal_deger == "guvenlikaktif") guvenlik_sistemi = true;
  else if (terminal_deger == "guvenlikpasif") guvenlik_sistemi = false;
  else if (terminal_deger == "sistemzamani")Serial.println(getTarihSaat());
  else if (terminal_deger == "kayitlar")Kayitlar();
  else if (terminal_deger == "sifredegis") {
    Serial.print("Yeni Sifre:");
    String yeni_sifre = DegerAl();

    Serial.print("Sifre Dogrula:");
    String dogrulama = DegerAl();
    if (yeni_sifre == dogrulama) {
      sifre = terminal_deger;
      Serial.println("Sifre Degisti .!");
    }
    else
      Serial.println("Sifre Dogrulanamadi .!");
  }
  else if (terminal_deger == "tarihayarla") {
    Serial.print("YIL:");
    String yil = DegerAl();

    Serial.print("AY:");
    String ay = DegerAl();

    Serial.print("GUN:");
    String gun = DegerAl();

    Serial.print("SAAT:");
    String saat = DegerAl();

    Serial.print("DAKIKA:");
    String dakika = DegerAl();

    Serial.print("SANIYE:");
    String saniye = DegerAl();

    setTime(saat.toInt(), dakika.toInt(), saniye.toInt(), gun.toInt(), ay.toInt(), yil.toInt());
    Serial.println(getTarihSaat());

  }
  else islem = false;

  return islem;
}

String DegerAl() {
  String deger;
  while (true) {
    if (Serial.available()) {
      int data = Serial.read();
      if (data == 13) {
        Serial.println();
        return deger;
      }
      else {
        deger = deger + (char) data;
        Serial.print((char)data);
      }
    }
  }
}

boolean GuvenlikKontrol() {
  boolean durum = false ;
  if ( digitalRead(22) == HIGH || digitalRead(23) == HIGH  ) {
    Serial.println("GUVENLIK IHLALI , YANGIN ALGILANDI .!");
    durum = true;
  }
  if ( digitalRead(24) == HIGH || digitalRead(25) == HIGH  ) {
    Serial.println("GUVENLIK IHLALI , HAREKET ALGILANDI .!");
    durum = true;

  }
  if ( digitalRead(26) == LOW || digitalRead(27) == LOW  ) {
    Serial.println("GUVENLIK IHLALI , PENCERE ACILDI .!");
    durum = true;
  }
  return durum;
}
