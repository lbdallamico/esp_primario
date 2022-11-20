#include "../include/settings_ht.h"

const uint8_t Pinout [5] = {LED_RED, LED_GRE, PIN_12V, PIN_5V, PIN_CAN};
// REPLACE WITH THE MAC Address of your receiver BOX 2
const uint8_t broadcastAddress[6] = {0xC0, 0x49, 0xEF, 0xCB, 0x4F, 0x38};
DeviceBox caixa_1;
void write_led_feedback(EVENT_SYSTEM _event, FEEDBACK_TEST result_test);
void write_event_on_pin(ROUTINE_TEST _event);
/*
 * Implementação dos testes
 */
static bool negador = 0;
bool ContinuityNegador = 0;
static bool semaphoro_time = 1;
hw_timer_t *My_timer = NULL; /*Usar para interrup timer*/
const uint64_t period_duct_cycle = (1/30)*1000000 /* ms */;
/*
 * Função para ser chamada na interrupção
 */
void Test__Init_Duct_Cycle(void);

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
  #endif
  caixa_1.Initializer((uint8_t*)broadcastAddress);
  Serial.println("INICIOU");
  digitalWrite(PIN_12V, HIGH);
  write_led_feedback(STARTING, ANY_TEST_RUNNING);
}
/*
 * 
 */
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
  write_event_on_pin(caixa_1.getRoutine());
  write_led_feedback(caixa_1.getSystemEvent(),
                     caixa_1.getFeedbackTest());
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
      digitalWrite(PIN_12V, HIGH);
      digitalWrite(PIN_5V, HIGH);
      digitalWrite(PIN_CAN, HIGH);
      if (!semaphoro_time)
      {
        timerAlarmDisable(My_timer);
        semaphoro_time = 1;
      }
      break;
    case DUCT_CYCLE:
      if (semaphoro_time)
      {
        My_timer = timerBegin(0, 80, true);
        timerAttachInterrupt(My_timer, &Test__Init_Duct_Cycle, true);
        timerAlarmWrite(My_timer, period_duct_cycle, true);
        timerAlarmEnable(My_timer); //Just Enable
        /*
          Para garantir que não haja reconfiguração
        */
        semaphoro_time = 0;
      }
      break;
    case SENOIDAL:
      if (!semaphoro_time)
      {
        timerAlarmDisable(My_timer);
        semaphoro_time = 1;
      }
      break;
    break;
  }
}

void write_led_feedback(EVENT_SYSTEM _event, FEEDBACK_TEST result_test)
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
      if (result_test == ANY_TEST_RUNNING)
      {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GRE, negador);
      }
      break;
    break;
  }
  if (_event == PERIPHEL_ON)
  {
    if (
      result_test == TEST_1_PASS ||
      result_test == TEST_2_PASS ||
      result_test == TEST_3_PASS
      )
    {
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GRE, HIGH);
    }
    else
    {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GRE, LOW);
    }
  }
}

void Test__Init_Duct_Cycle(void)
{
  digitalWrite(PIN_12V, ContinuityNegador);
  digitalWrite(PIN_5V, ContinuityNegador);
  digitalWrite(PIN_CAN, ContinuityNegador);
  ContinuityNegador = !ContinuityNegador;
}