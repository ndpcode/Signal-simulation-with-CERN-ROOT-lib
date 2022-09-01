# ADC signal simulation with CERN ROOT library

## Описание проекта
Программа для эмуляции цифрового сигнала и его преобразований. Для решения прикладной задачи потребовалось моделировать цифровой сигнал с выхода АЦП и два этапа его преобразования фильтрами. Исходный сигнал АЦП и промежуточные должны отрисовываться на графике с возможностью изменять частоту и амплитуду.

- Для отрисовки графиков сигналов и для возможности расширенного анализа данных была выбрана библиотека CERN ROOT.
- Реализован класс SignalGraphFrame, в который инкапсулированы алгоритмы, работа с ROOT и создание пользовательского интерфейса.
- Основной параметр при создании объекта SignalGraphFrame - функтор, описывающий три эмулируемых сигнала. После проведения эмуляции сигналов и отладки, исходный код, описывающий преобразования сигналов и фильтры, переносится в проект embedded устройства.
- В версии 1.0 часть параметров для эмуляции сигналов хранится внутри класса SignalGraphFrame в виде констант, далее необходимо сделать эти поля изменяемыми.
- Проект Visual Studio. Запуск из-под ROOT - "main run.bat".

## Демонстрация пользовательского интерфейса
![Image 1](Images/Image1.png?raw=true)
![Image 2](Images/Image2.png?raw=true)
