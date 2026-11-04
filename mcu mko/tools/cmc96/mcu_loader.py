# Memory emulator loader
# Mikhail Shomov shomovmy@iss-reshetnev.ru
# Version 1.0 Q3-2024

import sys
import os
import argparse
import serial
import serial.tools.list_ports
from struct import pack

parser = argparse.ArgumentParser(
   description='Loader of the MCU memory emulator'
  ,epilog='Mikhail Shomov, JSC "RESHETNEV", Q3-2024'
)
parser.add_argument(
   '--com'
  ,help='Set COM port (default COM3)'
  ,default='COM3'
)
parser.add_argument(
   '--start'
  ,help='Set program offset (default 0x2000)'
  ,default='0x2000'
)
parser.add_argument(
   '--bus'
  ,help='Set bus width {8, 16} (default 8)'
  ,default='8'
)
parser.add_argument(
   'binary_path'
  ,help="Path to binary file of MCU program"
)
arguments = parser.parse_args()

COM_port = serial.Serial(
   arguments.com
  ,baudrate=115200
  ,parity=serial.PARITY_NONE
  ,stopbits=serial.STOPBITS_ONE
  ,bytesize=serial.EIGHTBITS
  ,xonxoff=0
)

commands = {
   "RESET_ON"       : bytearray([0xA0, 0x00])
  ,"RESET_OFF"      : bytearray([0xA0, 0xFF])
  ,"ADDR_START"     : bytearray([0xB0])
  ,"BUSMODE_8"      : bytearray([0xC0, 0x00])
  ,"BUSMODE_16"     : bytearray([0xC0, 0x01])
  ,"CRC"            : bytearray([0xC1])         # unsupported on 14/04/24 (MSh)
  ,"RST_ON_ERR_ON"  : bytearray([0xC2, 0x00])   # unsupported on 14/04/24 (MSh)
  ,"RST_ON_ERR_OFF" : bytearray([0xC2, 0xFF])   # unsupported on 14/04/24 (MSh)
  ,"LENGTH"         : bytearray([0xD0])
  ,"ERR_DEST_NOTH"  : bytearray([0xE0, 0x00])   # unsupported on 14/04/24 (MSh)
  ,"ERR_DEST_UART"  : bytearray([0xE0, 0x01])   # unsupported on 14/04/24 (MSh)
  ,"ERR_DEST_GPIO"  : bytearray([0xE0, 0x10])   # unsupported on 14/04/24 (MSh)
  ,"ERR_DEST_BOTH"  : bytearray([0xE0, 0x11])   # unsupported on 14/04/24 (MSh)
}

def send_cmd(*command):
  uart_data = bytearray([0xCC])
  uart_data.extend(command[0])
  if (len(command) > 1):
    uart_data.append(command[1] & 0xFF)
    uart_data.append(0xCC)
    uart_data.append(command[0][0] | 0x01)
    uart_data.append(command[1] >> 8)
  COM_port.write(uart_data)

def send_data(filename):
  uart_data = bytearray([0xDD])
  with open(filename, 'rb') as file:
    uart_data.extend(file.read())
  send_cmd(commands["LENGTH"], len(uart_data)-1)
  COM_port.write(uart_data)

def main() -> int:
  bus_width = "BUSMODE_8" if (int(arguments.bus) == 8) else "BUSMODE_16"
  send_cmd(commands["RESET_ON"])
  send_cmd(commands[bus_width])
  send_cmd(commands["ADDR_START"], int(arguments.start, 16))
  send_data(arguments.binary_path)
  send_cmd(commands["RESET_OFF"])
  print("Done")
  return 0

if __name__ == '__main__':
  sys.exit(main())

