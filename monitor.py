import socket

if __name__ == '__main__':
	HOST='127.0.0.1'
	PORT=67 #should be 68
	BUFLEN=1024

	skt=socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	skt.bind(('', PORT))

	while True:
		print('Listening for messages on ' + HOST + ':' + str(PORT))
		data, addr = skt.recvfrom(BUFLEN)
		print('received message:\n'+str(data))
	
