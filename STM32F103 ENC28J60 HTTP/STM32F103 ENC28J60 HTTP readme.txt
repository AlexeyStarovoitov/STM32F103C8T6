STM32F103 ENC28J60 TCP Server: TCP Server с использованием HTTP между STM32F103, подключенного через Ethernet Controller ENC28J60, и ПК.
Скорость передачи Ethernet: 10 MБит/с.
Примечение: является продолжением проекта STM32F103 ENC28J60 UDP Client.
Краткое описание:
-Через Internet Explorer ПК устанавливает связь с STM32F103 по TCP-протоколу и шлет HTTP-запрос через TCP.
-STM32F103 отображает информацию через UART и назад шлет HTTP-страницу, которая отображается в Internet Explorer. 
-STM32F103 инициирует закрытие связи. 

