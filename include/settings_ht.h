#ifndef SETTINGS_HT_H
#define SETTINGS_HT_H

#define CAIXA1

// Global CPP defines
#include <iostream>

// Private includes
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include <Wire.h>

// Global defines
typedef enum
{
    INITIAL = 1,
    CONTINUITY,
    DUCT_CYCLE,
    SENOIDAL,
} ROUTINE_TEST;

typedef enum 
{
    STARTING = 0,
    ERROR_MODE,
    PERIPHEL_OFF,
    PERIPHEL_ON,
    TEST_PASS,
    TEST_FALL
} EVENT_SYSTEM;

typedef enum 
{
    DEAD = 0,
    ALIVE
} STATUS_BOX_2;

typedef enum 
{
    NO = 0,
    YES,
    ERRO,
} UpdateComunication;

typedef enum 
{
    NO_PRESSED = 0,
    PRESSED = 1,
} ButtonStatus;

// PINOUT LED
#if defined(CAIXA1)
    #define LED_RED     13
    #define LED_GRE     12
    #define PIN_12V     14
    #define PIN_5V      27
    #define PIN_CAN     26
    #define BUTTON_1    25
    #define BUTTON_2    33
    #define BUTTON_3    32 
#endif

typedef struct {
    EVENT_SYSTEM _event;
    ROUTINE_TEST _routine;
    STATUS_BOX_2 _box_alive;
} struct_msg;

// Global defines
class DeviceBox
{
    public:
        DeviceBox();
        ~DeviceBox();
        void Initializer(uint8_t * broadcast_address);
        void Send_Message(void);
        int Process(void);

        void setEventButton(ROUTINE_TEST button_press);
        ROUTINE_TEST getCurrentRoutine(void);
        EVENT_SYSTEM getCurrentStatus(void);

        static struct_msg _local_data;
        static struct_msg _recevid_data;
        static UpdateComunication _is_data_coming;
        static uint8_t _lost_box;
        esp_now_peer_info_t peerInfo;
        /*
            Callback function
            the idea is return communication status, we are using to make sure that box 2 is alive
        */
        static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
        static void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
    private:
        uint8_t __macAdress_receiver[6];
        bool snyc_esp(void);
};

#endif