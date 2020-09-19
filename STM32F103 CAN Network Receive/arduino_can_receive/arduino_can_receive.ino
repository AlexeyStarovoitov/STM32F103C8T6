
#include <mcp_can.h>
#include <SPI.h>

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif


const int SPI_CS_PIN = 10;
int leds[4];

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{

    SERIAL.begin(115200);
   
    while (CAN_OK != CAN.begin(CAN_500KBPS, MCP_8MHz))              // init can bus : baudrate = 500KBPS
    {
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println("Init CAN BUS Shield again");
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");

    for(int i = 0; i < 4; i++)
    {
      leds[i] = i + 4;
      pinMode(leds[i], OUTPUT);
      digitalWrite(leds[i], LOW);
    }
      
}

unsigned char len = 0;
unsigned char buf[8];
byte led_num = 0;

void loop()
{ 
  unsigned long canId_TX;
  while(CAN.checkReceive() != CAN_MSGAVAIL)
    ;
    
  CAN.readMsgBuf(&len, buf);
  unsigned long canId_RX = CAN.getCanId();


  for(int i = 0; i < len; i++)
      led_toggle(buf[i]);

  delay(1000);

//  if (canId_RX == 126)
//   canId_TX = 123;
//  else
//   canId_TX = canId_RX+1;
   canId_TX = canId_RX;
   
   led_num = buf[0];
   while (CAN.sendMsgBuf(canId_TX, 0, 0, 1, &led_num, true) != CAN_OK)
      ;
   
   
   SERIAL.println("CAN message transmitted!");
    
}

void led_toggle(int led_id)
{
  if (digitalRead(led_id) == HIGH)
    digitalWrite(led_id, LOW);
  else
    digitalWrite(led_id, HIGH);
}
