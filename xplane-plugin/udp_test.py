from socket import *
import sys

if __name__ == "__main__":
    sock = socket(AF_INET, SOCK_DGRAM)
    sock.sendto(chr(int(sys.argv[1])), ("localhost", 56244))
