# bB808_Drum
STM32F723 Disco based drum machine.

V 0.1
This is an STM32 based drum machine / sample player, on top of STM BSP and using STM32CubeIDE Version: 1.5.1 .
The 2 USB ports are used to connect an external MIDI pad ( at now LPD8 from AKAI ) on the FS port and a USB stick on the HS port.
Samples are stored in the board's QSPI, loaded from the USB stick.
The sampling frequency must be the same for all samples, now is tested with 22050KHz.
The maximum number of samples at now is 8 ( the LPD8 has 8 buttons, so ... ) and each sample uses 1MByte of QSPI flash.
The menus on the LCD can be navigated using an encoder connected on the board.
There are a number of things yet to be fixed or introduced.
All the function are based on the ST BSP and uses the ST functions. 

 
