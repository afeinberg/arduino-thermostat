#include <cassert>

#include "Arduino.h"
#include "DHT.h"

enum sensor_status_t {
    SENS_ERR_OK = 0,
    SENS_ERR_FAIL = -1,
    SENS_ERR_INFO = 2
};

enum {
    DEFAULT_SENS_PIN = 2,
    DEFAULT_SENS_TYPE = DHT22
};

enum {
    DEFAULT_DELAY_MILLIS = 2000
};

static
sensor_status_t __attribute__((warn_unused_result,nonnull))
read_sensor_data(DHT* dht,
                 float* temp,
                 float* humidity)
{
    float h = dht->readHumidity();
    if (isnan(h)) {
        return(SENS_ERR_FAIL);
    }
    float t = dht->readTemperature();
    if (isnan(t)) {
        return(SENS_ERR_FAIL);
    }
    *temp = t;
    *humidity = h;
    return(SENS_ERR_OK);
}

static
void  __attribute__((nonnull))
write_failure(Stream* out)
{
    out->println(SENS_ERR_FAIL);
}

static
void __attribute__((nonnull(1)))
write_success(Stream* out,
              float temp,
              float hum)
{
    out->print(SENS_ERR_OK);
    out->print(',');
    out->print(temp);
    out->print(',');
    out->println(hum);
}

static __attribute__((nonnull))
void write_info(Stream* out)
{
    out->print(SENS_ERR_INFO);
    out->print(',');
    out->println(DEFAULT_SENS_TYPE);
}

static
void __attribute__((nonnull))
sensor_setup(DHT* dht)
{
    Serial.begin(9600);
    dht->begin();
    write_info(&Serial);
}

static
void __attribute__((nonnull))
sensor_loop(DHT* dht)
{
    float hum;
    float temp;

    delay(DEFAULT_DELAY_MILLIS);

    switch (read_sensor_data(dht, &temp, &hum)) {
        case SENS_ERR_FAIL: {
            write_failure(&Serial);
            break;
        }
        case SENS_ERR_OK: {
            write_success(&Serial,temp, hum);
            break;
        }
        default: {
            assert(0);
        }
    }
}

int main()
{
    DHT dht(DEFAULT_SENS_PIN, DEFAULT_SENS_TYPE);

    init();

    sensor_setup(&dht);
    for (;;) {
        sensor_loop(&dht);
        if (serialEventRun) {
            serialEventRun();
        }
    }
    return(0);
}
