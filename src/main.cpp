#include "../include/settings_ht.h"
/* 
===========================================
            Global Variables
===========================================
*/
const uint8_t Pinout [10] = {LED_RED, 
                            LED_GRE, 
                            PIN_CABO_1, 
                            PIN_CABO_2, 
                            PIN_CABO_3, 
                            PIN_CABO_4,
                            LED_TEST_1,
                            LED_TEST_2,
                            LED_TEST_3,
                            LED_TEST_4};
const uint8_t broadcastAddress[6] = {0xC0, 0x49, 0xEF, 0xCB, 0x4F, 0x38};
DeviceBox caixa_1;
static bool negador = 0;
hw_timer_t * m_deboucer_timer = NULL;
/* 
===========================================
            Global Function 
===========================================
*/
void write_led_feedback(EVENT_SYSTEM _event, FEEDBACK_TEST result_test);
void write_event_on_pin(ROUTINE_TEST _event);
void write_pin_see_test(ROUTINE_TEST _event);
void IRAM_ATTR handler_time();
void Initializer__TIMER();
/* 
===========================================
            Portal Interrupt
===========================================
*/
void IRAM_ATTR Interrupt_botao1()
{
  Initializer__TIMER();
  caixa_1.ResetTest();
}

void IRAM_ATTR Interrupt_botao2()
{
  Initializer__TIMER();
  caixa_1.AvancaProxTest();
}
/* 
===========================================
                    SETUP
===========================================
*/
void setup()
{
  Serial.begin(115200);
  Serial.println();
  #if defined(CAIXA1)
    pinMode(BUTTON_1, INPUT_PULLDOWN);
    pinMode(BUTTON_2, INPUT_PULLDOWN);
    uint8_t i;
    for (i = 0; i < 10; i++)
    {
      pinMode(Pinout[i]     , OUTPUT);
      digitalWrite(Pinout[i], LOW);
    }
  #endif
  attachInterrupt(BUTTON_1, Interrupt_botao1, FALLING);
  attachInterrupt(BUTTON_2, Interrupt_botao2, FALLING);
  m_deboucer_timer = timerBegin(0, 80, true);
  caixa_1.Initializer((uint8_t*)broadcastAddress);
  Serial.println("INICIOU");
  write_led_feedback(STARTING, ANY_TEST_RUNNING);
}
/* 
===========================================
                    LOOP
===========================================
*/
void loop()
{
  caixa_1.Process();
  write_event_on_pin(caixa_1.getRoutine());
  write_pin_see_test(caixa_1.getRoutine());
  write_led_feedback(caixa_1.getSystemEvent(),
                     caixa_1.getFeedbackTest());
  caixa_1.Debug_SeeVariables();
  delay(DELAY_LOOP);
}
/*
===========================================
            Commom implementation
===========================================
*/
void write_event_on_pin(ROUTINE_TEST _event)
{
  switch(_event)
  {
    case INITIAL:
      ;
      break;
    case CONTINUIDADE_CABO_1:
      digitalWrite(PIN_CABO_1, HIGH);
      digitalWrite(PIN_CABO_2, LOW);
      digitalWrite(PIN_CABO_3, LOW);
      digitalWrite(PIN_CABO_4, LOW);
      break;
    case CONTINUIDADE_CABO_2:
      digitalWrite(PIN_CABO_1, LOW);
      digitalWrite(PIN_CABO_2, HIGH);
      digitalWrite(PIN_CABO_3, LOW);
      digitalWrite(PIN_CABO_4, LOW);
      break;
    case CONTINUIDADE_CABO_3:
      digitalWrite(PIN_CABO_1, LOW);
      digitalWrite(PIN_CABO_2, LOW);
      digitalWrite(PIN_CABO_3, HIGH);
      digitalWrite(PIN_CABO_4, LOW);
      break;
    case CONTINUIDADE_CABO_4:
      digitalWrite(PIN_CABO_1, LOW);
      digitalWrite(PIN_CABO_2, LOW);
      digitalWrite(PIN_CABO_3, LOW);
      digitalWrite(PIN_CABO_4, HIGH);
      break;
    break;
  }
}
void write_pin_see_test(ROUTINE_TEST _event)
{
  switch (_event)
  {
  case CONTINUIDADE_CABO_1:
    digitalWrite(LED_TEST_1, HIGH);
    digitalWrite(LED_TEST_2, LOW);
    digitalWrite(LED_TEST_3, LOW);
    digitalWrite(LED_TEST_4, LOW);
    break;
  case CONTINUIDADE_CABO_2:
    digitalWrite(LED_TEST_1, LOW);
    digitalWrite(LED_TEST_2, HIGH);
    digitalWrite(LED_TEST_3, LOW);
    digitalWrite(LED_TEST_4, LOW);
    break;
  case CONTINUIDADE_CABO_3:
    digitalWrite(LED_TEST_1, LOW);
    digitalWrite(LED_TEST_2, LOW);
    digitalWrite(LED_TEST_3, HIGH);
    digitalWrite(LED_TEST_4, LOW);
    break;
  case CONTINUIDADE_CABO_4:
    digitalWrite(LED_TEST_1, LOW);
    digitalWrite(LED_TEST_2, LOW);
    digitalWrite(LED_TEST_3, LOW);
    digitalWrite(LED_TEST_4, HIGH);
    break;
  default:
    digitalWrite(LED_TEST_1, LOW);
    digitalWrite(LED_TEST_2, LOW);
    digitalWrite(LED_TEST_3, LOW);
    digitalWrite(LED_TEST_4, LOW);
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
  if ((_event == PERIPHEL_ON) && (result_test != ANY_TEST_RUNNING))
  {
    if (
      result_test == TEST_1_PASS ||
      result_test == TEST_2_PASS ||
      result_test == TEST_3_PASS ||
      result_test == TEST_4_PASS
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
/* 
===========================================
            Handler Timer
===========================================
*/
void IRAM_ATTR handler_time()
{
  timerAlarmDisable(m_deboucer_timer);
  attachInterrupt(BUTTON_1, Interrupt_botao1, FALLING);
  attachInterrupt(BUTTON_2, Interrupt_botao2, FALLING);
}
void Initializer__TIMER()
{
  detachInterrupt(BUTTON_1);
  detachInterrupt(BUTTON_2);
  timerAttachInterrupt(m_deboucer_timer, &handler_time, true);
  timerAlarmWrite(m_deboucer_timer, 700000, true);
  timerAlarmEnable(m_deboucer_timer);
}
