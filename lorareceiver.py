# -*- coding: utf-8 -*-
"""
LORA receiver with timestamp and sender info
Unihiker M10 + Reyax Technology RYLR999 connected to 3.3, GND, P0 (TX) and P3 (RX)
Roni Bandini, November 2025
"""

import time
import os
from datetime import datetime
from unihiker import GUI
from pinpong.board import Board, Pin, NeoPixel, UART

Board("UNIHIKER").begin()
gui = GUI()

np1 = NeoPixel(Pin(Pin.P13), 3)
led = Pin(Pin.D15, Pin.OUT)
led.write_digital(0)

uart = UART(bus_num=0)
uart.init(baud_rate=115200, bits=8, parity=0, stop=1)

os.system('clear')
print("LoRa receiver\n")

np1.brightness(128)
np1.range_color(0, 2, 0x0000FF)
time.sleep(1)
np1.brightness(0)

def send_at(cmd):
    print(f"Sending: {cmd}")
    uart.write(list((cmd + "\r\n").encode()))

    start = time.time()
    response = b""
    got_data = False

    while time.time() - start < 1.5:
        if uart.any() > 0:
            chunk = uart.read(uart.any())
            if chunk:
                response += bytes(chunk)
                got_data = True
            time.sleep(0.05)
        elif got_data:
            break

    if response:
        print("Answer:", response.decode(errors="ignore").strip())
    else:
        print("*** No answer ***")

print("\nLORA Receiver - Roni Bandini, Nov 2025\n")

gui.clear()
gui.draw_text(x=5, y=10, text="LORA Receiver", font_size=15, color="black", origin='left')
gui.draw_text(x=5, y=30, text="Roni Bandini", font_size=15, color="black", origin='left')


send_at("AT+ADDRESS=7")
send_at("AT+NETWORKID=18")
print("Waiting for messages...\n")

line = 50
max_lines = 180   

while True:
    if uart.any() > 0:
        raw = uart.readline()
        if not raw:
            continue

        try:
            msg = bytes(raw).decode(errors="ignore").strip()
        except:
            continue

        if not msg:
            continue

        print(f"RAW: [{msg}]")
        data = msg
        sender = "?"

        if msg.startswith("+RCV="):
            np1.brightness(128)
            np1.range_color(0, 2, 0x00FF00)

            parts = msg.split(',')
            # Estructura: +RCV=<addr>,<len>,<data>,<RSSI>,<SNR>
            if len(parts) >= 3:
                try:
                    sender = parts[0].split('=')[1]
                    data = parts[2]
                except:
                    sender = "?"
                timestamp = datetime.now().strftime("%H:%M:%S")
                display_text = f"{timestamp} [{sender}]: {data}"

                print(display_text)

                gui.draw_text(x=5, y=line, text=timestamp+" ("+sender+") "+data, font_size=13, color="black", origin='left')
                line += 20
                if line > max_lines:
                    gui.clear()
                    gui.draw_text(x=5, y=10, text="LORA Receiver", font_size=15, color="black", origin='left')
                    gui.draw_text(x=5, y=30, text="Roni Bandini", font_size=15, color="black", origin='left')
                    line = 50

                time.sleep(2)
                np1.brightness(0)

    time.sleep(0.05)
