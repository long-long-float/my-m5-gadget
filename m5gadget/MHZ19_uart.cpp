/*
  MHZ19_uart.cpp - MH-Z19 CO2 sensor library for ESP-WROOM-02/32(ESP8266/ESP32) or Arduino
  version 0.3
  
  License MIT
*/

#include "MHZ19_uart.h"
#include "Arduino.h"

#define WAIT_READ_TIMES 100
#define WAIT_READ_DELAY 10

// public
MHZ19_uart::MHZ19_uart()
{
}
MHZ19_uart::MHZ19_uart(int rx, int tx)
{
	begin(rx, tx);
}

MHZ19_uart::~MHZ19_uart()
{
}

void MHZ19_uart::begin(int rx, int tx)
{
	_rx_pin = rx;
	_tx_pin = tx;
}

void MHZ19_uart::setAutoCalibration(boolean autocalib)
{
	writeCommand(autocalib ? autocalib_on : autocalib_off);
}

int MHZ19_uart::getCO2PPM()
{
	readSerialData();
	return _co2;
}

#ifdef ARDUINO_ARCH_ESP32
void MHZ19_uart::setHardwareSerialNo(int serialNo)
{
	_serialNo = serialNo;
}
#endif

//protected
void MHZ19_uart::writeCommand(uint8_t cmd[])
{
	writeCommand(cmd, NULL);
}

void MHZ19_uart::writeCommand(uint8_t cmd[], uint8_t *response)
{
#ifdef ARDUINO_ARCH_ESP32
	HardwareSerial hserial(_serialNo);
	hserial.begin(9600, SERIAL_8N1, _rx_pin, _tx_pin);
#else
	SoftwareSerial hserial(_rx_pin, _tx_pin);
	hserial.begin(9600);
#endif
	hserial.write(cmd, REQUEST_CNT);
	hserial.write(mhz19_checksum(cmd));
	hserial.flush();

	if (response != NULL)
	{
		int i = 0;
		while (hserial.available() <= 0)
		{
			if (++i > WAIT_READ_TIMES)
			{
				Serial.println("error: can't get MH-Z19 response.");
				return;
			}
			yield();
			delay(WAIT_READ_DELAY);
		}
		hserial.readBytes(response, MHZ19_uart::RESPONSE_CNT);
	}
}

//private
void MHZ19_uart::readSerialData()
{
	uint8_t buf[MHZ19_uart::RESPONSE_CNT];
	for (int i = 0; i < MHZ19_uart::RESPONSE_CNT; i++)
	{
		buf[i] = 0x0;
	}

	writeCommand(getppm, buf);

/*
  Serial.print("readSerialData: ");
  for (int i = 0; i < MHZ19_uart::RESPONSE_CNT; i++) {
    Serial.print(buf[i]);
    Serial.print(", ");
  }
  Serial.println();
*/

	// parse
	if (buf[0] == 0xff && buf[1] == 0x86 && mhz19_checksum(buf) == buf[MHZ19_uart::RESPONSE_CNT - 1])
	{
		_co2 = buf[2] * 256 + buf[3];
	}
	else
	{
		_co2 = -1;
	}
}

uint8_t MHZ19_uart::mhz19_checksum(uint8_t com[])
{
	uint8_t sum = 0x00;
	for (int i = 1; i < MHZ19_uart::REQUEST_CNT; i++)
	{
		sum += com[i];
	}
	sum = 0xff - sum + 0x01;
	return sum;
}
