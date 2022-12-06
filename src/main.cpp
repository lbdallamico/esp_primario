#include "../include/settings_ht.h"

const uint8_t Pinout [6] = {LED_RED, LED_GRE, PIN_CABO_1, PIN_CABO_2, PIN_CABO_3, PIN_CABO_4};
// REPLACE WITH THE MAC Address of your receiver BOX 2
const uint8_t broadcastAddress[6] = {0xC0, 0x49, 0xEF, 0xCB, 0x4F, 0x38};
DeviceBox caixa_1;
void write_led_feedback(EVENT_SYSTEM _event, FEEDBACK_TEST result_test);
void write_event_on_pin(ROUTINE_TEST _event);

uint8_t gambi_led = 0;
/*
 * Implementação dos testes
 */
static bool negador = 0;
static uint8_t deboucer_botao = 0;

void IRAM_ATTR Interrupt_botao1()
{
  caixa_1.ResetTest();
  deboucer_botao = 3;
  detachInterrupt(BUTTON_1);
  detachInterrupt(BUTTON_2);
}

void IRAM_ATTR Interrupt_botao2()
{
  caixa_1.AvancaProxTest();
  deboucer_botao = 3;
  detachInterrupt(BUTTON_1);
  detachInterrupt(BUTTON_2);
}

void deboucer_de_loop()
{
  if (deboucer_botao > 1)
  {
    deboucer_botao--;
  }
  else if (deboucer_botao == 1)
  {
    attachInterrupt(BUTTON_1, Interrupt_botao1, FALLING);
    attachInterrupt(BUTTON_2, Interrupt_botao2, FALLING);
    deboucer_botao--;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  #if defined(CAIXA1)
    pinMode(BUTTON_1, INPUT_PULLDOWN);
    pinMode(BUTTON_2, INPUT_PULLDOWN);
    uint8_t i;
    for (i = 0; i < 6; i++)
    {
      pinMode(Pinout[i]     , OUTPUT);
      digitalWrite(Pinout[i], LOW);
    }
  #endif
  attachInterrupt(BUTTON_1, Interrupt_botao1, FALLING);
  attachInterrupt(BUTTON_2, Interrupt_botao2, FALLING);
  caixa_1.Initializer((uint8_t*)broadcastAddress);
  Serial.println("INICIOU");
  write_led_feedback(STARTING, ANY_TEST_RUNNING);
}

void loop()
{
  caixa_1.Process();
  write_event_on_pin(caixa_1.getRoutine());
  write_led_feedback(caixa_1.getSystemEvent(),
                     caixa_1.getFeedbackTest());
  caixa_1.Debug_SeeVariables();
  deboucer_de_loop();
  delay(DELAY_LOOP);
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

bool timer_neg = 0;
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
      result_test == TEST_3_PASS
      )
    {
      /*
      if(gambi_led>0)
      {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GRE, timer_neg);
        timer_neg = !timer_neg;
        gambi_led--;
      } */
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