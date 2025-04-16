#include "DHT.h"

#define DHTPIN 2       // Broche connectée au capteur
#define DHTTYPE DHT11  // Type du capteur (DHT11)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);  // Démarre la communication série
  dht.begin();         // Initialise le capteur
}

void loop() {
  float temperature = dht.readTemperature(); // Lit la température
  float humidite = dht.readHumidity();       // Lit l'humidité

  // Vérifie si les mesures sont valides
  if (!isnan(temperature) && !isnan(humidite)) {
    // Envoie sous le format : 23.50;60.00\n
    Serial.print(temperature);
    Serial.print(";");
    Serial.println(humidite);
  } else {
    Serial.println("Erreur de lecture du capteur");
  }

  delay(1000); // Attend 10 secondes avant la prochaine mesure
}

