import socket 
import smtplib
from email.message import EmailMessage
from datetime import datetime
import requests
import mysql.connector

mydb = mysql.connector.connect(
	host = "localhost",
	user = "brestovacki.e192",
	password = "3-Sk*-Ta",
	database = "db_brestovacki_e192"
)

HOST = ''
PORT = 50067
NUM_OF_CLIENTS = 1
tcpSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcpSocket.bind((HOST, PORT))
tcpSocket.listen(NUM_OF_CLIENTS)
print('Echo server is ready to receive (port ' + str(PORT) + ')\n')

msgCnt = 1
while True:
    try:
        conn, addr = tcpSocket.accept()
        print('Connected by', addr)
        server = smtplib.SMTP_SSL('smtp.gmail.com', 465)
        mycursor = mydb.cursor()
        mycursor.execute('SELECT email FROM `door` ')
        records = mycursor.fetchall()
        for row in records:
                to = row[0]
        print(to)
        msg = EmailMessage()
        while True:
            data_in = conn.recv(1024)
            if not data_in:
                break
            data_out = data_in.upper()
            now = datetime.now()

            current_time = now.strftime("%H:%M:%S")
            dat_str = str(data_in)
            pos = len(str(data_in))-1
            server.login('lenkatestlenka@gmail.com', 'bhcx wbpy hocu mvki')
            msg.set_content(dat_str[2:pos]+" u "+current_time)
            #msg.set_content("nesto")
            msg['Subject'] = 'VRATA'
            msg['From'] = "lenkanestolenka@gmail.com"
            for row in records:
                    to = row[0]
                    msg['To'] = to
                    server.send_message(msg)
                    del msg['To']
            print("logged")
            #server.sendmail('lenkatestlenka@gmail.com', 'l.brestovacki@gmail.com', msg)
            print("sent mail")
            server.quit()
            print(' Rx: ',data_in)
            print(' Tx: ',data_out)
            conn.sendall(data_out)
    except:
        conn.close()
        print('ERROR in Echo TCP')