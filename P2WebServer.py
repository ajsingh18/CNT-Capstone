#Name: Ian Rey Baguio
#Class: Capstone 2960
#Instructor: Marc Anderson

#***************CODE MUST RUN IN PYTHON 2*************
import bluetooth
import requests


bd_addr = "20:17:07:25:48:65" #address from the Arduino sensor
port = 1
sock = bluetooth.BluetoothSocket (bluetooth.RFCOMM)
sock.connect((bd_addr,port))

data = ""

while 1:
        try:
            data += sock.recv(1024)
            data_end = data.find('\n')
            if data_end != -1:
                rec = data[:data_end]
                url = 'https://thor.net.nait.ca/~ibaguio1/CAPSTONE/IRTEMP/webService.php' #url where to send temp data
                query = {'temp': data}
                res = requests.post(url, data=query)
                data = data[data_end+1:]
                print(res.text)
                print(data)
        except KeyboardInterrupt:
            break
sock.close()

##exTemp = "24.97"
##
##url = 'https://thor.net.nait.ca/~ibaguio1/CAPSTONE/IRTEMP/webService.php'
##query = {'temp': exTemp}
##res = requests.post(url, data=query)
##print(res.text)
