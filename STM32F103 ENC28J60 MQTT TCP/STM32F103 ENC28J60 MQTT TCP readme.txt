STM32F103 ENC28J60 MQTT TCP: осуществление передачи сообщений MQTT с помощью библиотеки net.c/enc28j60 и MQTT-C.
Изменения:
-Были убраны мютексы, так как у нас будет один поток.
-В качестве структуры mqtt_pal_socket_handle была использована структура с сохранением параметров TCP-пакетов: порядковый номер и номер подтверждения, порты.

Задачи:
-Реализовать прием-передачу данных через структуру сокета в файлах net.h/net.c.
Socket работает следующим образом:
1. При чтении пакетов на каждом уровне (eth, ip, tcp, udp) в поля dst записывать адреса устройства, от которого пришло сообщение.
2. При отправке пакетов на каждом уровне записывать из этого сокета в пакеты соответствующих интерфейсов.
-Реализовать функции mqtt_pal_sendall и mqtt_pal_recvall через функции передачи  net.c/enc28j60. Нужно написать функцию tcp_request.
-Реализовать обмен данными через брокер Orange Pi с облаком AWS IoT.
