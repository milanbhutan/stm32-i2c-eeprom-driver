# STM32_I2C_EEPROM_Driver
Developed a UART-based interface for reading and writing data via I2C to an EEPROM memory chip

# Features
- UART-based interface which allows users to type in a string of characters to be written to the EEPROM at a specified address. The user can also request the number of bytes to be read from a specified address
- Register-level I2C communication
- Supports 15-bit memory addressing

# Hardware Used
- STM32L4A6GZ
- 24LC256 I2C EEPROM
- UART connection to PC (USB Serial)

# Hardware Layout
<img width="737" height="580" alt="image" src="https://github.com/user-attachments/assets/849eb941-464f-44ea-a121-588578e055f3" />

# How to Run
1. Flash firmware onto STM32 board
2. Connect UART to PC (e.g., via USB serial)
3. Wire STM32 to EERPOM as shown in the hardware layout
4. Open serial terminal such as PuTTY (115200 baud)
5. Follow the instructions shown on the terminal to write or read bytes

# Logic Analyzer Output
<img width="880" height="524" alt="image" src="https://github.com/user-attachments/assets/8daad469-8630-4e67-b8ac-fc15881afd1a" />

# Datasheets
- [24LC256 I2C EEPROM Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/20001203U.pdf)
- [STM32L4 Reference Manual (RM0351)](https://www.st.com/resource/en/reference_manual/dm00083560.pdf)
