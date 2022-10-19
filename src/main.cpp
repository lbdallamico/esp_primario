#include "../include/settings_ht.h"

bool negador = 0;

uint8_t Pinout [5] = {LED_RED, LED_GRE, PIN_12V, PIN_5V, PIN_CAN};
// REPLACE WITH THE MAC Address of your receiver BOX 2
uint8_t broadcastAddress[6] = {0xC0, 0x49, 0xEF, 0xCB, 0x4F, 0x38};

DeviceBox caixa_1;

void write_led_feedback(EVENT_SYSTEM _event);
void write_event_on_pin(ROUTINE_TEST _event);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  #if defined(CAIXA1)
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GRE, OUTPUT);
    pinMode(PIN_12V, OUTPUT);
    pinMode(PIN_5V , OUTPUT);
    pinMode(PIN_CAN, OUTPUT);
    pinMode(BUTTON_1, INPUT_PULLDOWN);
    pinMode(BUTTON_2, INPUT_PULLDOWN);
    pinMode(BUTTON_3, INPUT_PULLDOWN);
    uint8_t i;
    for (i = 0; i < sizeof(Pinout) - 1; i++)
    {
      digitalWrite(Pinout[i], LOW);
    }
  # elif defined(CAIXA2)
    pinMode(PIN_12V, INPUT_PULLDOWN);
    pinMode(PIN_5V , INPUT_PULLDOWN);
    pinMode(PIN_CAN, INPUT_PULLDOWN);
  #endif
  caixa_1.Initializer(broadcastAddress);
  Serial.println("INICIOU");
  digitalWrite(PIN_12V, HIGH);
  write_led_feedback(STARTING);
}

int gambi_force_button = 0;
ROUTINE_TEST vector_force_button[] = {CONTINUITY, DUCT_CYCLE, SENOIDAL};
int index_v = 0;

void loop()
{
  if (gambi_force_button > 10)
  {
    gambi_force_button=0;
    caixa_1.setEventButton(vector_force_button[index_v]);
    if (index_v > 1)
    {
      index_v = 0;
    } 
    else
    {
      index_v++;
    }
  }
  if (digitalRead(BUTTON_1) == PRESSED)
  {
    caixa_1.setEventButton(CONTINUITY);
  }
  else if (digitalRead(BUTTON_2) == PRESSED)
  {
    caixa_1.setEventButton(DUCT_CYCLE);
  }
  else if (digitalRead(BUTTON_3) == PRESSED)
  {
    caixa_1.setEventButton(SENOIDAL);
  }

  caixa_1.Process();
  write_event_on_pin(caixa_1.getCurrentRoutine());
  write_led_feedback(caixa_1.getCurrentStatus());
  caixa_1.Debug_SeeVariables();

  gambi_force_button++;

  delay(500);
}

/*
  Commom implementation
*/
void write_event_on_pin(ROUTINE_TEST _event)
{
  switch(_event)
  {
    case INITIAL:
      ;
      break;
    case CONTINUITY:
      ;
      break;
    case DUCT_CYCLE:
      ;
      break;
    case SENOIDAL:
      ;
      break;
    break;
  }
}

void write_led_feedback(EVENT_SYSTEM _event)
{
  negador = !negador;
  switch(_event)
  {
    case ERROR_MODE:
      digitalWrite(LED_RED, negador);
      digitalWrite(LED_GRE, negador);
      break;
    case STARTING:
      /*
        Evento mestre
      */
      digitalWrite(LED_RED, HIGH);
      delay(500);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GRE, HIGH);
      delay(500);
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GRE, LOW);
      delay(500);
      digitalWrite(LED_RED, LOW);
      break;
    case PERIPHEL_OFF:
      digitalWrite(LED_RED, negador);
      digitalWrite(LED_GRE, LOW);
      break;
    case PERIPHEL_ON:
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GRE, negador);
      break;
    /*
    case TEST_PASS:
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GRE, HIGH);
      break;
    case TEST_FALL:
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GRE, LOW);
      break;
    */
    break;
  }
}