Репозиторий проекта
ows-auv-chersonese
Operator Work Station Autonomous Underwater Vehicle Chersonese
https://github.com/valabsoft/ows-auv-chersonese.git

Фильтр WireShark для отладки пакетов
ip.addr == 127.0.0.1 and udp.port in {2525}

Иконки GUI
https://www.iconfinder.com/search/icons?family=knowledge

Описание проекта
Текущий проект SevROV состоит из подпроектов:
SevROVControl -- Старая программы управления
SevROVEmulator -- Емулятор приема команд управления и передачи ответа телеметрии
SevROVLibrary -- Библиотека вспомогательных классов
SevROVWorkstation -- Новая версия программы управления (АРМ оператора)
SevROVCalibration -- Утилита для калибровки камер

Зависимости и библиотеки:
OpenCV 4.8.0 (cборка под MinGW)
SDL2 2.28.2 (cборка под MinGW)
MVS (Machine Visison Camera SDK) 4.0.0

Состав классов SevROVLibrary
SevROVConnector - класс для передачи команд на SevROV и получения ответа телеметрии
SevROVControlData - класс для описание пакета команд управления
SevROVTelemetryData - класс для описания пакета данных телеметрии
SevROVData - служебный класс для пакетов данных и телеметрии. Является родительским для SevROVControlData и SevROVTelemetryData
SevROVLibrary - главный класс библиотеки, содержит единственный метод XboxToControlData - преобразование команд от джойстика в команды управления (нужен рефакторинг)
SevROVPIDController - класс для описания настроек ПИД-контроллера
SevROVXboxController - класс для работы с джойстиком Xbox

ТНПА
Трансляция и запись видео в проекте SevROV не производится. Нужно перенести из проекта Катран
https://github.com/valabsoft/sargan-cv-control.git
см. SarganYOLO модуль main.cpp MJPEGStreamer streamer; 
см. // Создаем объект для записи видео
