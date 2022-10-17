#include "settings_ht.h"

struct_msg DeviceBox::_local_data;
struct_msg DeviceBox::_recevid_data;
UpdateComunication DeviceBox::_is_data_coming;
uint8_t DeviceBox::_lost_box = 0;

DeviceBox::DeviceBox(void){
    /*
        Construtor apenas para garantir espaço na memoria
    */
   ;
}

void DeviceBox::Initializer(uint8_t * broadcast_address)
{
    // clear trash values from struct
    // I cannot use memcpy because this variable are static!
    _local_data._box_alive = DEAD;
    _local_data._event = STARTING;
    _local_data._routine = INITIAL;
    _recevid_data._box_alive = DEAD;
    _recevid_data._event = STARTING;
    _recevid_data._routine = INITIAL;

    _lost_box = 0;

    memcpy(&__macAdress_receiver, broadcast_address, sizeof(__macAdress_receiver));
    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        _local_data._event = ERROR_MODE;
        return;
    }
    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(DeviceBox::OnDataSent);
    // Register peer
    memcpy(peerInfo.peer_addr, __macAdress_receiver, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        _local_data._event = ERROR_MODE;
        return;
    }
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(DeviceBox::OnDataRecv);
}

DeviceBox::~DeviceBox(){}

void DeviceBox::setEventButton(ROUTINE_TEST button_press)
{
    _local_data._routine = button_press;
    // The idea is shows to box that has update on status
    Send_Message();
}

ROUTINE_TEST DeviceBox::getCurrentRoutine(void)
{
    return _local_data._routine;
}

EVENT_SYSTEM DeviceBox::getCurrentStatus(void)
{
    return _local_data._event;
}

void DeviceBox::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if (status == ESP_OK)
    {
        _lost_box = 0;
    }
    else
    {
        _lost_box++;
        _is_data_coming = ERRO;
    }
}

void DeviceBox::OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
    memcpy(&_recevid_data, incomingData, sizeof(_recevid_data));
    _is_data_coming = YES;
}

void DeviceBox::Send_Message(void)
{
    esp_err_t result = esp_now_send(__macAdress_receiver, 
                                    (uint8_t *) &_local_data, 
                                    sizeof(_local_data));
    if (result == ESP_OK) {
        Serial.println("Sent with success");
    }
    else {
        Serial.println("Error sending the data");
    }
}

/*
    @brief: Verifica se as esp estão sincronizadas através
            dos eventos e rotinas
    @param: Nothing
    @return: TRUE  - Esp estão sincronizadas
             FALSE - Desincronizadas
*/
bool DeviceBox::snyc_esp(void)
{
    if (_local_data._event != _recevid_data._event) return 0;
    else return 1;
}

int DeviceBox::Process(void)
{
    /*
        Semrpre que houver qualquer alteração no sistema
        seja estado ou evento, os dois sistemas precisam
        ficar sabendo
    */
    if (_lost_box > 3)
    {
        _local_data._event = ERROR_MODE;
    }
    if (_is_data_coming == YES)
    {
        _is_data_coming = NO;
        if (_local_data._box_alive = ALIVE)
        {
            _local_data._event = PERIPHEL_ON;
            return PERIPHEL_ON;
        }
        else if (_local_data._box_alive = DEAD)
        {
            _local_data._event = PERIPHEL_OFF;
            return PERIPHEL_OFF;
        }
    }
    else if (snyc_esp() == 0 || _is_data_coming == ERRO)
    {
        Send_Message();
    }
    return -1;
}