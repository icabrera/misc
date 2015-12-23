import serial
import time
import random

debug_flag = 1

def send_command( str ):
  if debug_flag == 1:
    print "TX: " + str
  ser.write(str + '\r\n')
  return

def read_answer():
  out = ''
  while ser.inWaiting() > 0:
    out += ser.read(1)
  return out

server_port = 80
ser = serial.Serial(port='/dev/ttyUSB0', baudrate=9600, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)

#Opening serial port
ser.isOpen()

#Resetting module
send_command("AT+RST")
time.sleep(1)
out = read_answer()
if out.find("OK") != -1:
  print "RX: " + "OK"
else:
  print "RX: " + "No answer"

#Verifiyng module by sending AT and waiting OK.
while 1:
  send_command("AT")
  time.sleep(1)
  out = read_answer()
  if out.find("OK") != -1:
    print "RX: " + "OK"
    break
  else:
    print "RX: " + "No answer"

#Changing mode
send_command("AT+CIPMUX=1")
time.sleep(1)
out = read_answer()
if out.find("OK") != -1:
  print "RX: " + "OK"
else:
  print "RX: " + "No answer"

#Start server
send_command("AT+CIPSERVER=1," + str(server_port))
time.sleep(1)
out = read_answer()
if out.find("OK") != -1:
  print "RX: " + "OK"
else:
  print "RX: " + "No answer"

#Wait for WiFi connection by reading IP address periodically. Exit when address is != 0.0.0.0
while 1:
  send_command("AT+CIFSR")
  time.sleep(1)
  out = read_answer()
  direcciones_ip = out.split()
  if direcciones_ip[2].find("0.0.0.0") == -1:
    print "IP: " + direcciones_ip[2]
    break

#Receiving packets in a forever loop.
while 1:
  while 1:
      request = ser.readline()
      if request.find("GET") != -1:
        arguments = request.split()
        
        if arguments[1] == '/':
          filename = "main.html"
        else:
          filename = arguments[1][1:]

        print "GET " + str(filename)
        requerido = 1
        break

  time.sleep(1)

  #Empty buffer.
  out = read_answer()

  if requerido == 1:

    #In this example, script is only attending the request of main.html, any other file will be reported as not found.
    if filename == "main.html":
      respuesta = "<html><head><title>Prueba ESP8266</title></head><body><h2>La temperatura es:" + str(random.random()*100) + "</h2></body></html>"
    else:
      respuesta = "<html><head><title>Prueba ESP8266</title></head><body><h2>File not found</h2></body></html>"

    command = "AT+CIPSEND=0," + str(len(respuesta)+2)
    send_command(command)

    time.sleep(0.1)
    out = read_answer()
    if out.find(">") != -1:
      
      send_command(respuesta)
      time.sleep(0.25)

      out = read_answer()
      if out.find("SEND OK") != -1:
        send_command("AT+CIPCLOSE=0")
      else:
        print "RX: No answer"