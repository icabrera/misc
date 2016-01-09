char estado;
char estado_siguiente;

char estado_temp;
char estado_siguiente_temp;

#define PRENDE 1
#define APAGA 2
#define ESPERA 3

#define LEE_TEMPERATURA 4
#define LEE_HUMEDAD 5
#define VALIDA_LECTURAS 6

#define TIEMPO_PRENDIDO_DEF 500
#define TIEMPO_APAGADO_DEF 500
#define TIEMPO_ESPERA_SENSOR 2000

int led_estado = 0;
int tiempo_actual = 0;
int tiempo_anterior = 0;

int tiempo_actual_temp = 0;
int tiempo_anterior_temp = 0;

int ledPin=13;
char data_ready = 0;

#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);


float h=0;
float t=0;

void setup()
{
    led_on_off_setup();
    sensor_temp_setup();
    serial_write_setup();
}

void loop()
{
    led_on_off_loop();
    sensor_temp_loop();
    serial_write_loop();
}


void led_on_off_setup()
{
    pinMode(ledPin, OUTPUT);
    estado = PRENDE;
}

void led_on_off_loop()
{
    if (estado == PRENDE)
    {
        //Guardo el estado del LED
        led_estado = HIGH;
        //Prendo el LED
        digitalWrite(ledPin, led_estado);
        //Guardo en el tiempo en el que se prendio el LED.
        tiempo_anterior = millis();
        //Paso al estado de espera.
        estado_siguiente = ESPERA;
    }
    else if (estado == ESPERA)
    {
        tiempo_actual = millis() - tiempo_anterior;
        if (led_estado == HIGH)
        {
            if (tiempo_actual >= TIEMPO_PRENDIDO_DEF)
            {
                estado_siguiente = APAGA;
            }
        }
        else if (led_estado == LOW)
        {
            if (tiempo_actual >= TIEMPO_APAGADO_DEF)
            {
                estado_siguiente = PRENDE;
            }
        }
    }
    else if (estado == APAGA)
    {
        //Guardo el estado del LED
        led_estado = LOW;
        //Prendo el LED
        digitalWrite(ledPin, led_estado);
        //Guardo en el tiempo en el que se prendio el LED.
        tiempo_anterior = millis();
        //Paso al estado de espera.
        estado_siguiente = ESPERA;
    }
    estado = estado_siguiente;
}


void sensor_temp_setup()
{
    dht.begin();
    estado_temp = ESPERA;
    estado_siguiente_temp = ESPERA;
    tiempo_anterior_temp = millis();
}


void sensor_temp_loop()
{
    if (estado_temp == ESPERA)
    {
        if ( millis() - tiempo_anterior_temp  >= TIEMPO_ESPERA_SENSOR)
        {
            estado_siguiente_temp = LEE_TEMPERATURA;
        }
    }
    else if (estado_temp == LEE_TEMPERATURA)
    {
        t = dht.readTemperature();
        estado_siguiente_temp = LEE_HUMEDAD;
    }
    else if (estado_temp == LEE_HUMEDAD)
    {
        h = dht.readHumidity();
        estado_siguiente_temp = VALIDA_LECTURAS;
    }
    else if (estado_temp == VALIDA_LECTURAS)
    {

        if ( ! (isnan(h) || isnan(t)) )
        {
            data_ready = 1;
        }
        else
        {
            data_ready = 0;
        }
        estado_siguiente_temp = ESPERA;
        tiempo_anterior_temp = millis();
    }
    estado_temp = estado_siguiente_temp;
}


void serial_write_setup()
{
    Serial.begin(115200);
}

void serial_write_loop()
{
    if (data_ready == 1)
    {
        data_ready = 0;
        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.println(" *C ");
    }
}

