STM32F103 ENC28J60 UDP Client: UDP Client между STM32F103, подключенного через Ethernet Controller ENC28J60, и ПК.
Скорость передачи Ethernet: 10 MБит/с.
Примечение: является продолжением проекта STM32F103 ENC28J60 UDP Server.
Краткое описание:
-Через netcat ПК устанавливает связь с STM32F103 по UDP-протоколу и шлет ему информацию.
-STM32F103 отображает через UART порт ПК и пользователь шлет запрос в формате "xxx.xxx.xxx.xxx:portu"
-STM32F103 шлет сначала ARP-запрос и узнает MAC-адрес ПК, а затем посылает UDP-запрос с нужной информацией.
-UDP-запрос от STM32F103 отображается в netcat. 

