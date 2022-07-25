# Qt-PID-tuning-regulator

- Qt Cross-platform software for tuning PID parameters, based on analyzed input data
- Communication via Serialport with Arduino
- Realitime plotting realized by QCustomPlot


# Qt Application

![screenshot_25_07_22](https://user-images.githubusercontent.com/106441011/180827398-5047a06e-c63b-4554-84b7-177e3d4435c7.png)

Responsive design application ready for deploy on various embedded platforms (such as Raspberry Pi)

Done:
- UART communication (search for available ports, choose Baudrate, connect)
- Log system
- Realtime plotting based on received data from MCU



TO DO:
- add graph features such as pointers, hover mouseover etc
- add combobox with auto-tuning methods (such as Ziegler-Nichols)
- separate manual PID parameters setting and automatical PID tuning
- saving test results in CSV file


# Embedded

Electronic parts:
- Arduino Uno board
- DFRobot DC Motor Driver 2x15A_lite
- 7-12 V DC brush motor with gearbox 40:1
- encoder Okystar KY-040

Done:
- velocity control regulator (close loop, PID)
- implemented low-pass filter with 25Hz cutoff (encoder velocity measuring)
- UART communication (each message sent is simmilar to CSV (comma separated values) line)
- changing paremeters in "realtime" based on received msg from Qt Application

TO DO:
- change type of encoder (for magnetic or optic)
- add position control regulator (close loop, PID)

Issues:
- due to using mechanical encoder Okystar KY-040, connected to output shaft of motor gearbox, resolution of measured velocity is not like it should be (40 ticks per 1 output shaft revolution), easy fix for that should be changing position of encoder (connect with other gear in gearbox) or type of encoder
- with higher baudrate measured velocity signal is very noisy, changing setting for low-pass filter should help, but with current encoder it's no point to implement this
