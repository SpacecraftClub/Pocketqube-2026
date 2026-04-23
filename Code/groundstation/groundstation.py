# SPDX-FileCopyrightText: 2018 Brent Rubell for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
Simple ground station for PocketQube sensor telemetry.

Learn Guide: https://learn.adafruit.com/lora-and-lorawan-for-raspberry-pi
Author: Brent Rubell for Adafruit Industries
"""
# Import Python System Libraries
import struct
# Import Blinka Libraries
import busio
from digitalio import DigitalInOut
import board
# Import the SSD1306 module.
import adafruit_ssd1306
# Import RFM9x
import adafruit_rfm9x

FIELD_NAMES = (
    "pressure_pa",
    "temp_ms5611_c",
    "mag_x",
    "mag_y",
    "mag_z",
    "accel_x",
    "accel_y",
    "accel_z",
    "gyro_x",
    "gyro_y",
    "gyro_z",
    "temp_lsm",
)
PACKET_FORMAT = "<H12f"
PACKET_LEN = struct.calcsize(PACKET_FORMAT)

# Create the I2C interface.
i2c = busio.I2C(board.SCL, board.SDA)

# 128x32 OLED Display
reset_pin = DigitalInOut(board.D4)
display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, reset=reset_pin)
# Clear the display.
display.fill(0)
display.show()
width = display.width
height = display.height

# Configure LoRa Radio
CS = DigitalInOut(board.CE1)
RESET = DigitalInOut(board.D25)
spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
rfm9x = adafruit_rfm9x.RFM9x(spi, CS, RESET, 915.0)
rfm9x.signal_bandwidth = 125000
rfm9x.coding_rate = 7
rfm9x.spreading_factor = 12

# Draw a box to clear the image
display.fill(0)
display.text("Qube Telemetry", 16, 0, 1)
display.show()

def decode_packet(packet):
    if len(packet) != PACKET_LEN:
        return None

    values = struct.unpack(PACKET_FORMAT, packet)
    seq_num = values[0]
    telemetry = dict(zip(FIELD_NAMES, values[1:]))
    return seq_num, telemetry

def loop():
    packet = rfm9x.receive()
    if packet is None:
        display.fill(0)
        display.text("Waiting for PKT", 8, 0, 1)
        display.show()
        return

    decoded = decode_packet(packet)
    display.fill(0)
    if decoded is None:
        print("Unexpected packet length:", len(packet))
        display.text("Bad packet len", 0, 0, 1)
        display.text(str(len(packet)), 0, 12, 1)
        display.show()
        return

    seq_num, telemetry = decoded
    print("seq=", seq_num, telemetry)

    display.text("Seq {}".format(seq_num), 0, 0, 1)
    display.text("P {:.0f}".format(telemetry["pressure_pa"]), 0, 12, 1)
    display.text("T {:.1f}".format(telemetry["temp_ms5611_c"]), 64, 12, 1)
    display.text("Az {:.2f}".format(telemetry["accel_z"]), 0, 24, 1)
    display.show()

def main():
    while True:
        loop()

main()
