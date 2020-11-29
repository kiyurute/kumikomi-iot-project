#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <WiFi.h>
#include <String.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "esp_system.h"

#define LEDPIN 15
#define cdsPin 33
#define TEMPHUMIDSENSOR_PIN 4

const uint16_t kIrLed = 5;
IRsend irsend(kIrLed);

char *ssid = "Buffalo-G-B6D1";
char *password = "srnfvihira8sn";
 
const char *endpoint = "a2knz1o7g8a6nz-ats.iot.ap-northeast-1.amazonaws.com";
// Example: xxxxxxxxxxxxxx.iot.ap-northeast-1.amazonaws.com
const int port = 8883;
char *pubTopic = "$aws/things/MyESP32/shadow/update";
char *subTopic = "$aws/things/MyESP32/shadow/update/delta";
 
const char* rootCA = "-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";
 
const char* certificate = "-----BEGIN CERTIFICATE-----\n" \
"MIIDWTCCAkGgAwIBAgIUVHWAiPg4lIB0LtyrYHDqH35KWowwDQYJKoZIhvcNAQEL\n" \
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n" \
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIwMDQyMzA4Mjc0\n" \
"NFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n" \
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKjBuiUXxW7QU4zdBDyQ\n" \
"crRZbnRIypxbl4NM1+VUI2IX24QBMJTpMuFYoQA95BiTfYLezmr2I41xR4m1LMaP\n" \
"zN+4j2/XKyeg9SxSDlNptFtQQRwX22DvQeRNyx1M2Z4kyCQ1pjjq+2/AzJbTmIWE\n" \
"Ux+F0laouOrap5BVGYj9aBLpPTOtQJ/UPJ+ethqeTRAaX6+qizjrJ64f8FinNvbG\n" \
"YmYUAnkXek3mteXzulEHWDgDc57tj+4YHCM5dh4xWWs9h8EIdePXQP7YS8t6Iplv\n" \
"H7/TyFM4VSWxsN51k2DPu2qH/JJAnZdDJhCn3m45Prupt/vAxNpZBANIOr6UzRtu\n" \
"eyECAwEAAaNgMF4wHwYDVR0jBBgwFoAUK1aB9c/G6YdK7s7eqLk8G2NS++0wHQYD\n" \
"VR0OBBYEFMc3lyI5JEKGzTz2LHGjl3vS62t5MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n" \
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBz1W7aL82y0U2lEhWAx2i4Xk08\n" \
"/1pegTOGqhad6825S/txDnFIF9eR3uzKwUxvZCQkH23j9ZkCXyl24FbKPBwRvqvZ\n" \
"wLudA3VO1ALEzHCAr8DE9iUTy9DBQIG9dT0hTgaI598K0+6FxUepBFjx2aLBUMLa\n" \
"tHyM6llUnrUrhsVtZWGW2caWMbcWDS25emNBs3I4oiwBVdrpWYKDOE0bTf2Q07Dz\n" \
"UMkdpiuAmPkJ2L/8N5FkqHaJJNdo6zJHd1kjJctYcqxqCcxMv43djU6idvcz8/BK\n" \
"iOrdE8d3UfAWQ0PnqGoofIPsTb6RN1wVwkVWhbafjXb0Jc4bWUE5SLT5FC6X\n" \
"-----END CERTIFICATE-----\n";
 
const char* privateKey = "-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpAIBAAKCAQEAqMG6JRfFbtBTjN0EPJBytFludEjKnFuXg0zX5VQjYhfbhAEw\n" \
"lOky4VihAD3kGJN9gt7OavYjjXFHibUsxo/M37iPb9crJ6D1LFIOU2m0W1BBHBfb\n" \
"YO9B5E3LHUzZniTIJDWmOOr7b8DMltOYhYRTH4XSVqi46tqnkFUZiP1oEuk9M61A\n" \
"n9Q8n562Gp5NEBpfr6qLOOsnrh/wWKc29sZiZhQCeRd6Tea15fO6UQdYOANznu2P\n" \
"7hgcIzl2HjFZaz2HwQh149dA/thLy3oimW8fv9PIUzhVJbGw3nWTYM+7aof8kkCd\n" \
"l0MmEKfebjk+u6m3+8DE2lkEA0g6vpTNG257IQIDAQABAoIBAANsEktIRR+vJ9KW\n" \
"dMyJnVnc6ml/RGSRUuw9ta8X2rPVigUORR7v77E0K+emWabJ7uSlgggXP7dRkcLQ\n" \
"Ir1QEzaCNp5s/ZoLII2qiFHWteqYIQnhzfgUQgDQNws8ELmtQ0XAtMM4uKqWfU+d\n" \
"sN0cRtc/znklaMKSdo1tvFvTz7V6buo+JKWw7ornTHWp8Vzi5JMLSyuYYi0isrNz\n" \
"Y52v5Bp3gWlUxhQqwJ5M8wsOwwMQkVU3PfDXMG5tftIvTEI/FlHPSDSprptoLTB2\n" \
"xprLek2ETcd8LcLPB4d5AkEI6zdBVTT+OqZt6lUMvqPyKlEl8c5rsFwMgsJHTld2\n" \
"nVQdn9UCgYEA3NvLv/Dq6XuhZkqcVTfc1q1ne5GEiR2OqBdfNfZEM8Ja8rHD8j5X\n" \
"y8oE6jd4LZguxAP7JvSAD2ThiI9ma5FBVOwVOf/a5y3bl1ge2RVKCvWwD6FPr7hM\n" \
"45OMBjF7aYJP91vAJdeXmBpf1kUyjOad+5x7RJkPoRDbP8VGcHAwkssCgYEAw5us\n" \
"jTxhSSffYoElZ/xnoa1W3D6BcDwHZLwoySAxtHIZyInXyV9WhDMhmZXXMfpA3X3x\n" \
"OWFk85VUBwvIuYuFC/D1KEDaZYylheeETaTgGi8mvxefl/InSgSAC4oF33Qm+vDh\n" \
"PBm1z0TbpKHXELyILlYfZ0OfhwI1YnVdnplWMEMCgYEAvcb4KzRfBsW/CGCwOOHJ\n" \
"l4ZlV17RHekR/gonO/0i2b7123k4wF0ln+01j/SeBb+w36n0E6MgrxmmWmuuPFwS\n" \
"YWCDVNUvSlqzSDbmOu76MrSu1p9sE7gwCZVtSp0WSLE6+XgTgfDhSEfRdhL7vusj\n" \
"cWMi4O0M6v/LBd8ECfW9J0UCgYEAuCfjt8RUww7q3E5B7ThQSIJs+EVQj120iD4t\n" \
"F81bfkq9w3Mi2M8NIMESrQnI46B7WQutAxySclDwHwIVI7oatDdSV2dXNavIWdyx\n" \
"TngfFAe4LsgK1H6CRskd079J8CnmWPJW2sJJzK1DLCfB1lvoydcDEbE0i2Kk5+Ef\n" \
"FqW94wkCgYAsctI0H6lTbiS5tSw6YcqsrxRrIF/AYwbw7wMtYAOOPxUBabPE/uQB\n" \
"49YSu0JE1yPkqsF3unLHt+L3CqueS6KAbSi9onqLtYHq3iZ9Gv6CUiZzfulLFrkv\n" \
"Hg9h92KFoerddcRgQsmyAXk3+L/tMRduk22giv9b8wC7PeeryjX92Q==\n" \
"-----END RSA PRIVATE KEY-----\n";
 
WiFiClientSecure httpsClient;
PubSubClient mqttClient(httpsClient);

DHT dht(TEMPHUMIDSENSOR_PIN, DHT11);

float humidity;
float temperature; 
int cdsValue;
int DI;

int rightCounter=100;
int darkCounter=0;

int HhumCounter=1000;
int goodHumCounter=100;
int LhumCounter=0;

int highDI=1000;
int goodDI=0;

//right off
uint16_t right_off[643] = {4708, 2586,  610, 912,  608, 1810,  608, 1812,  606, 914,  608, 1812,  608, 1810,  608, 912,  662, 858,  606, 914,  608, 1812,  606, 1812,  608, 914,  606, 1812,  608, 1812,  608, 1810,  608, 910,  662, 858,  608, 1812,  608, 914,  606, 1810,  610, 910,  608, 912,  660, 1758,  608, 910,  608, 910,  608, 912,  608, 912,  608, 912,  606, 912,  606, 912,  608, 910,  608, 906,  608, 16416,  4708, 2586,  608, 912,  608, 1812,  608, 1810,  608, 912,  608, 1810,  608, 1810,  662, 858,  608, 912,  608, 912,  608, 1812,  608, 1810,  662, 858,  608, 1810,  608, 1812,  632, 1786,  610, 910,  608, 912,  660, 1756,  608, 914,  606, 1810,  608, 910,  608, 912,  662, 1756,  608, 912,  608, 910,  610, 912,  608, 910,  662, 858,  608, 912,  608, 910,  608, 912,  608, 908,  606};

//right on
uint16_t right_on[643] = {4764, 2534,  660, 914,  606, 1786,  632, 1814,  606, 886,  632, 1760,  658, 1814,  606, 886,  632, 914,  606, 860,  658, 1788,  632, 1786,  632, 914,  606, 1788,  632, 1786,  632, 1812,  606, 914,  606, 914,  606, 888,  632, 1786,  632, 886,  658, 888,  606, 888,  632, 1786,  632, 886,  632, 886,  632, 914,  604, 888,  632, 914,  604, 888,  632, 888,  630, 862,  660, 882,  630, 16360,  4758, 2560,  578, 942,  632, 1788,  576, 1842,  576, 942,  578, 1842,  578, 1868,  550, 942,  578, 916,  604, 942,  578, 1842,  578, 1842,  576, 944,  576, 1842,  576, 1842,  576, 1842,  576, 942,  578, 942,  578, 942,  578, 1840,  578, 940,  578, 942,  578, 942,  578, 1840,  578, 942,  578, 916,  604, 942,  578, 940,  578, 916,  604, 942,  578, 916,  604, 916,  604, 936,  578, 16412,  4706, 2588,  604, 916,  604, 1814,  606, 1814,  604, 916,  604, 1814,  606, 1812,  606, 912,  606, 912,  606, 914,  608, 1814,  606, 1812,  606, 914,  606, 1814,  606, 1812,  608, 1812,  606, 912,  606, 912,  608, 914,  606, 1812,  606, 914,  606, 912,  606, 914,  606, 1812,  606, 912,  606, 912,  606, 912,  608, 914,  606, 912,  606, 914,  606, 912,  606, 912,  608, 908,  606};

//AC on
uint16_t ac_on[643] = {496, 392,  476, 392,  476, 392,  476, 394,  476, 392,  422, 25230,  3520, 1698,  476, 1264,  478, 390,  476, 392,  422, 446,  476, 1262,  422, 446,  478, 392,  476, 394,  476, 392,  474, 1264,  476, 392,  474, 1264,  476, 1262,  478, 392,  476, 1264,  474, 1262,  476, 1262,  476, 1262,  478, 1262,  476, 394,  474, 394,  474, 1264,  422, 446,  476, 392,  478, 392,  504, 364,  476, 394,  476, 392,  476, 394,  476, 392,  476, 392,  422, 446,  422, 1316,  476, 392,  476, 394,  474, 392,  504, 366,  422, 446,  476, 394,  476, 394,  422, 446,  476, 392,  476, 392,  474, 394,  476, 392,  476, 394,  474, 394,  474, 394,  476, 392,  476, 394,  476, 392,  476, 392,  476, 394,  476, 392,  422, 446,  474, 394,  476, 392,  504, 366,  476, 392,  476, 392,  476, 392,  476, 394,  476, 394,  474, 394,  476, 392,  476, 394,  478, 392,  474, 394,  476, 392,  478, 392,  476, 392,  478, 392,  476, 390,  424, 446,  476, 394,  476, 394,  504, 366,  474, 394,  476, 394,  422, 446,  422, 446,  476, 394,  422, 446,  478, 392,  478, 392,  420, 448,  476, 394,  476, 392,  476, 392,  474, 394,  478, 392,  476, 392,  474, 394,  478, 392,  476, 392,  476, 394,  476, 392,  476, 394,  476, 392,  478, 392,  476, 394,  474, 392,  476, 392,  478, 392,  474, 394,  474, 392,  478, 392,  476, 392,  478, 392,  476, 392,  476, 392,  478, 392,  476, 394,  476, 392,  476, 392,  476, 392,  476, 392,  476, 392,  476, 394,  502, 366,  474, 394,  476, 392,  476, 392,  478, 392,  504, 364,  504, 364,  478, 394,  474, 392,  506, 364,  476, 392,  476, 394,  476, 392,  476, 392,  476, 392,  476, 392,  478, 392,  476, 394,  476, 392,  476, 394,  476, 392,  476, 392,  478, 392,  476, 392,  476, 392,  476, 394,  476, 392,  476, 392,  476, 392,  476, 392,  476, 394,  504, 364,  502, 366,  476, 1262,  476, 1264,  502, 366,  476, 390,  476, 1262,  478, 392,  476, 392,  504, 364,  480, 34736,  3550, 1670,  476, 1262,  502, 366,  478, 392,  476, 392,  478, 1260,  504, 364,  478, 392,  478, 392,  476, 392,  476, 1260,  478, 394,  474, 1262,  504, 1234,  476, 394,  476, 1262,  478, 1262,  502, 1236,  478, 1262,  478, 1260,  476, 392,  476, 394,  478, 1260,  476, 392,  420, 448,  476, 392,  476, 392,  476, 394,  476, 392,  478, 390,  478, 392,  476, 392,  476, 392,  476, 392,  476, 392,  504, 364,  504, 364,  476, 392,  504, 366,  478, 392,  476, 392,  476, 1262,  478, 392,  478, 392,  476, 1262,  476, 1262,  502, 1236,  478, 392,  476, 392,  478, 392,  476, 392,  474, 1262,  478, 392,  476, 1262,  502, 1236,  476, 392,  478, 392,  476, 392,  478, 392,  504, 364,  478, 392,  478, 392,  476, 392,  478, 392,  504, 366,  502, 366,  476, 394,  474, 394,  476, 392,  476, 1262,  482, 1256,  482, 388,  476, 1262,  474, 394,  476, 392,  476, 392,  504, 364,  478, 392,  476, 392,  476, 392,  476, 394,  502, 364,  476, 392,  476, 392,  478, 392,  474, 394,  476, 394,  476, 392,  476, 392,  476, 392,  476, 1262,  478, 1260,  478, 394,  476, 394,  476, 392,  476, 392,  504, 366,  476, 392,  476, 390,  478, 392,  476, 392,  476, 394,  474, 1262,  476, 1262,  476, 392,  504, 366,  476, 394,  476, 392,  476, 392,  476, 392,  476, 394,  504, 364,  504, 364,  476, 392,  476, 394,  476, 392,  476, 394,  476, 392,  476, 392,  476, 392,  476, 392,  476, 1262,  476, 394,  476, 392,  476, 394,  504, 364,  476, 392,  476, 1262,  476, 1262,  478, 392,  478, 390,  478, 392,  476, 392,  478, 1262,  476, 392,  478, 390,  478, 1260,  504, 364,  504, 366,  476, 392,  476, 392,  476, 392,  476, 392,  476, 392,  476, 392,  478, 392,  476, 1262,  476, 1262,  504, 364,  476, 392,  504, 1236,  504, 1234,  504, 1234,  478};

//AC off
uint16_t ac_off[643] = {480, 392,  502, 366,  504, 366,  476, 392,  476, 394,  476, 25174,  3550, 1668,  504, 1236,  504, 366,  474, 392,  476, 392,  478, 1262,  504, 366,  502, 364,  478, 392,  476, 392,  478, 1258,  506, 366,  504, 1234,  502, 1234,  478, 392,  504, 1234,  504, 1234,  534, 1204,  506, 1232,  478, 1262,  476, 392,  476, 394,  474, 1262,  478, 390,  504, 366,  504, 364,  504, 366,  502, 364,  482, 390,  474, 392,  504, 366,  476, 392,  504, 366,  476, 1260,  504, 366,  504, 366,  502, 366,  478, 392,  504, 366,  476, 392,  478, 392,  504, 366,  504, 364,  476, 392,  502, 366,  502, 366,  504, 366,  476, 392,  476, 394,  474, 392,  504, 364,  478, 392,  478, 392,  502, 364,  504, 366,  476, 392,  504, 364,  476, 394,  502, 364,  476, 394,  476, 392,  502, 366,  480, 390,  476, 392,  476, 392,  476, 392,  478, 390,  504, 364,  504, 366,  478, 392,  502, 368,  504, 364,  478, 392,  506, 364,  476, 390,  478, 392,  502, 366,  504, 366,  476, 392,  504, 366,  476, 392,  478, 392,  478, 392,  476, 394,  504, 364,  476, 392,  476, 392,  476, 394,  502, 366,  476, 392,  504, 364,  504, 366,  478, 392,  476, 392,  474, 394,  504, 364,  476, 394,  504, 364,  476, 392,  502, 366,  476, 394,  476, 392,  504, 364,  478, 392,  504, 364,  504, 364,  478, 392,  502, 364,  476, 392,  476, 392,  478, 390,  504, 366,  478, 392,  476, 392,  504, 364,  504, 366,  476, 392,  476, 392,  476, 392,  478, 390,  478, 392,  478, 390,  478, 392,  476, 392,  504, 366,  476, 390,  504, 366,  502, 366,  478, 392,  476, 394,  476, 392,  502, 366,  476, 394,  476, 392,  504, 364,  476, 392,  504, 364,  476, 392,  476, 392,  478, 392,  504, 362,  506, 366,  504, 364,  476, 392,  478, 390,  478, 390,  476, 394,  502, 366,  504, 364,  504, 364,  478, 392,  476, 392,  502, 366,  476, 1262,  504, 1236,  478, 390,  504, 364,  504, 1234,  478, 390,  478, 392,  504, 364,  478, 34740,  3526, 1694,  478, 1260,  476, 392,  476, 392,  478, 392,  476, 1262,  478, 392,  476, 392,  504, 366,  502, 364,  478, 1262,  476, 392,  476, 1260,  506, 1234,  478, 392,  478, 1260,  476, 1262,  476, 1262,  476, 1262,  474, 1264,  504, 364,  476, 392,  476, 1262,  478, 392,  422, 446,  480, 388,  502, 368,  474, 394,  476, 396,  474, 392,  476, 392,  474, 394,  476, 392,  476, 392,  422, 446,  474, 394,  502, 366,  476, 392,  476, 392,  424, 446,  476, 392,  476, 394,  476, 392,  422, 446,  476, 1262,  476, 1262,  424, 1316,  422, 446,  476, 394,  422, 446,  422, 446,  476, 1262,  478, 392,  422, 1316,  424, 1316,  422, 446,  476, 392,  478, 392,  476, 392,  478, 392,  422, 446,  476, 392,  422, 448,  422, 448,  422, 446,  478, 392,  476, 392,  422, 446,  478, 392,  422, 1316,  424, 1314,  424, 446,  476, 1262,  424, 446,  476, 394,  422, 446,  476, 394,  422, 446,  476, 392,  422, 446,  422, 448,  422, 446,  424, 448,  420, 446,  422, 446,  422, 446,  476, 392,  476, 392,  476, 392,  422, 446,  422, 1316,  422, 1316,  424, 446,  422, 446,  422, 448,  422, 446,  422, 446,  422, 448,  422, 446,  422, 446,  422, 446,  422, 446,  422, 1316,  422, 1314,  422, 448,  422, 446,  422, 446,  422, 446,  422, 446,  422, 446,  422, 446,  422, 446,  422, 446,  422, 448,  422, 446,  422, 448,  422, 446,  422, 446,  422, 446,  422, 446,  422, 446,  422, 1316,  422, 446,  422, 448,  422, 446,  422, 446,  422, 448,  422, 1316,  422, 1316,  422, 446,  422, 448,  422, 446,  422, 446,  422, 1316,  422, 446,  422, 446,  422, 1316,  426, 442,  422, 446,  422, 446,  422, 446,  422, 448,  422, 446,  422, 446,  422, 448,  422, 1316,  422, 448,  422, 1316,  422, 446,  422, 446,  422, 1316,  422, 1316,  422, 1316,  422};


//about watch dog----------------------------------------------------------------------

const int wdtTimeout = 10*60*1000;  //time in ms to trigger the watchdog   10分
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}


void setup() {
    Serial.begin(115200);
 
    // Start WiFi
    Serial.println("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected.");
 
    // Configure MQTT Client
    httpsClient.setCACert(rootCA);
    httpsClient.setCertificate(certificate);
    httpsClient.setPrivateKey(privateKey);
    mqttClient.setServer(endpoint, port);
    mqttClient.setCallback(mqttCallback);
 
    connectAWSIoT();

    pinMode(LEDPIN,OUTPUT);
    pinMode(cdsPin,INPUT);

    dht.begin();
    irsend.begin();

    timer = timerBegin(0, 80, true);                  //timer 0, div 80
    timerAttachInterrupt(timer, &resetModule, true);  //attach callback
    timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
    timerAlarmEnable(timer);
}


 
void connectAWSIoT() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP32_device")) {
            Serial.println("Connected.");
            int qos = 0;
            mqttClient.subscribe(subTopic, qos);
            Serial.println("Subscribed.");
        } else {
            Serial.print("Failed. Error state=");
            Serial.print(mqttClient.state());
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
 
long messageSentAt = 0;
int dummyValue = 0;
char pubMessage[128];
 
void mqttCallback (char* topic, byte* payload, unsigned int length) {
    Serial.print("Received. topic=");
    Serial.println(topic);
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.print("\n");

    Serial.println((char)payload[2]);  //なぜか必要

    if((char)payload[2]=='a'){
    Serial.println("LEDをつけます");
    digitalWrite(LEDPIN,HIGH);
    }else if((char)payload[2]=='b'){
    Serial.println("LEDを消します");
    digitalWrite(LEDPIN,LOW);
    }else if((char)payload[2]=='c'){
    Serial.println("電気をつけます");
    irsend.sendRaw(right_on, 643, 38);  // Send a raw data capture at 38kHz
    }else if((char)payload[2]=='d'){
    Serial.println("電気を消します");
    irsend.sendRaw(right_off, 643, 38);  // Send a raw data capture at 38kHz.
    }else if((char)payload[2]=='e'){
    Serial.println("エアコンをつけます");
    irsend.sendRaw(ac_on, 643, 38);  // Send a raw data capture at 38kHz.
    delay(5);
    irsend.sendRaw(ac_on, 643, 38);  // Send a raw data capture at 38kHz.
    }else if((char)payload[2]=='f'){
    Serial.println("エアコンを消します");
    irsend.sendRaw(ac_off, 643, 38);  // Send a raw data capture at 38kHz.
    delay(5);
    irsend.sendRaw(ac_off, 643, 38);  // Send a raw data capture at 38kHz.
    }
}
 
void mqttLoop() {
    if (!mqttClient.connected()) {
        connectAWSIoT();
    }
    mqttClient.loop();
 
    long now = millis();
    if (now - messageSentAt > 5000) {

//        humidity = dht.readHumidity();
//        temperature = dht.readTemperature();
//        cdsValue=analogRead(cdsPin);
//
//        Serial.print("temperature:");Serial.println(temperature);
//        Serial.print("hunidity:");Serial.println(humidity);
        
  if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        //ESP.restart();
    }else{}
    
        messageSentAt = now;
        //sprintf(pubMessage, "{\"state\": {\"desired\":{\"foo\":\"%d\"}}}", dummyValue++);
        sprintf(pubMessage, "{\"cdsValue\":\"%d\", \"temperature\":\"%d\",\"humidity\":\"%d\"}",cdsValue,(int)temperature,(int)humidity);
        Serial.print("Publishing message to topic ");
        Serial.println(pubTopic);
        Serial.println(pubMessage);
        mqttClient.publish(pubTopic, pubMessage);
        Serial.println("Published.");
    }
}

String message;

void sendLineNotify() {
  const char* host = "notify-api.line.me";
  const char* token = "tRkxL9iyAKayxyHwQufy2isZ1PHkLpJzr45kVv7i0cR";
//  const char* message = "test"; //ごはんの絵文字をURLエンコードしたもの
  
  WiFiClientSecure client;
  Serial.println("Try");
  //LineのAPIサーバに接続
  if (!client.connect(host, 443)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Connected");
  //リクエストを送信
  String query = String("message=") + String(message);
  String request = String("") +
               "POST /api/notify HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Bearer " + token + "\r\n" +
               "Content-Length: " + String(query.length()) +  "\r\n" + 
               "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
                query + "\r\n";
  client.print(request);
 
  //受信終了まで待つ 
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  
  String line = client.readStringUntil('\n');
  Serial.println(line);
}


void loop() {
 
  timerWrite(timer, 0); 
 
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  cdsValue=analogRead(cdsPin);
  DI=0.81*temperature+0.01*humidity*(0.99*temperature-14.3)+46.3;

  Serial.print("temperature:");Serial.println(temperature);
  Serial.print("hunidity:");Serial.println(humidity);
  Serial.print("cdsValue:");Serial.println(cdsValue);

  delay(1000);

  
  //部屋の明るさの通知設定------------------------------------------------
  if(cdsValue>1500){
    rightCounter+=1;
    darkCounter=0;
    }else{
    darkCounter+=1;
    rightCounter=100;  
    }

   if(rightCounter==199){
    rightCounter=104;
    }

   if(rightCounter==99){
    rightCounter=4;
    }
   
   if(rightCounter==103){
    message="部屋が良い明るさだよ";
    sendLineNotify();
    }

    if(darkCounter==3){
    message="部屋が暗くなったよ";
    sendLineNotify();
    }

   //不快指数の通知設定------------------------------------------------
   if(DI>85){
    highDI+=1;
    goodDI=0;
    }else{
    highDI=100; 
    goodDI+=1; 
    }

   if(highDI==199){
    highDI=104;
    }

   if(goodDI==99){
    goodDI=4;
    }
   
   if(highDI==103){
    message="部屋の不快指数が高くなったよ";
    sendLineNotify();
    }

    if(goodDI==3){
    message="部屋の不快指数が良くなったよ";
    sendLineNotify();
    }
   
  
  mqttLoop();
}
