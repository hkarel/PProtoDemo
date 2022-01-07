# Демонстрационный проект с примерами использования протокола [PProto](https://github.com/hkarel/PProtoCpp)

Демонстрационный проект содержит примеры сериализации данных и передачи сообщений. Проект зависит от библиотек [SharedTools](https://github.com/hkarel/SharedTools), [PProtoCpp](https://github.com/hkarel/PProtoCpp), [RapidJson](https://github.com/hkarel/rapidjson), [Yaml-Cpp](https://github.com/hkarel/yaml-cpp), они подключены как субмодули.  
Демо-проект создан с использованием QtCreator, сборочная система QBS. Сценарии для сборки примеров:  

- pproto_demo_project.qbs - все примеры;
- pproto_demo_serialize.qbs - примеры сериализации данных;
- pproto_demo_transport.qbs - примеры по работе с сообщениями.

## Примеры сериализации

- SDemo 01 - Базовый пример бинарной сериализации;
- SDemo 02 - Пассивное версионирование;
- SDemo 03 - Избыточное версионирование;
- SDemo 04 - Агрегирование;
- SDemo 05 - Наследование;
- SDemo 06 - Базовый пример JSON сериализации;
- SDemo 07 - Пассивное версионирование;
- SDemo 08 - Агрегирование;
- SDemo 09 - Наследование;
- SDemo 10 - Маппинг наименований полей;
- SDemo 11 - Сериализация [адресной книги](https://developers.google.com/protocol-buffers/docs/cpptutorial) `protobuf` при помощи `pproto`. Используется двойная сериализация: `qbinary`, `json`;
- SDemo 12 - Пример [адресной книги](https://developers.google.com/protocol-buffers/docs/cpptutorial) c `protobuf`-сериализацией.

## Примеры работы с сообщениями

- TDemo 01 - Базовый пример создания сообщения;
- TDemo 02 - Обмен сообщениями между клиентом и сервером.

