#ifndef SETTINGS_HT_H
#define SETTINGS_HT_H

#define CAIXA1

// Global CPP defines
#include <iostream>
#include <vector>

// Private includes
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include <Wire.h>

// Global defines
typedef enum
{
    INITIAL = 0,
    CONTINUIDADE_CABO_1,
    CONTINUIDADE_CABO_2,
    CONTINUIDADE_CABO_3,
    CONTINUIDADE_CABO_4,
} ROUTINE_TEST;

typedef enum 
{
    STARTING = 0,
    ERROR_MODE,
    PERIPHEL_OFF,
    PERIPHEL_ON,
} EVENT_SYSTEM;

typedef enum
{
    ANY_TEST_RUNNING = 0,
    TEST_1_PASS,
    TEST_1_FAIL,
    TEST_2_PASS,
    TEST_2_FAIL,
    TEST_3_PASS,
    TEST_3_FAIL,
    TEST_4_PASS,
    TEST_4_FAIL,
} FEEDBACK_TEST;

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
    #define LED_RED     33
    #define LED_GRE     13
    #define PIN_CABO_1  26
    #define PIN_CABO_2  27
    #define PIN_CABO_3  25
    #define PIN_CABO_4  32
    #define BUTTON_1    12
    #define BUTTON_2    14
    #define LED_TEST_1  15
    #define LED_TEST_2  2
    #define LED_TEST_3  18
    #define LED_TEST_4  22
#endif

#define TIME_FEEDBACK_TEST  200
#define DELAY_LOOP          400

typedef struct {
    EVENT_SYSTEM    _event;
    ROUTINE_TEST    _routine;
    STATUS_BOX_2    _box_alive;
    FEEDBACK_TEST   _feedback_test;
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
        /*
            V2 implementações
        */
        void ResetTest(void);
        void AvancaProxTest(void);

        void setEventButton(ROUTINE_TEST button_press);
        ROUTINE_TEST getRoutine(void);
        EVENT_SYSTEM getSystemEvent(void);
        FEEDBACK_TEST getFeedbackTest(void);
        void Debug_SeeVariables(void);
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