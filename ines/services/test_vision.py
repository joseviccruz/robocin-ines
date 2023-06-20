# import socket
# import netifaces

# # Get a list of all network interfaces
# interfaces = netifaces.interfaces()

# # Iterate over each interface
# for interface in interfaces:
#     # Check if the interface supports multicast
#     if netifaces.AF_INET in netifaces.ifaddresses(interface):
#         # Get the IP addresses assigned to the interface
#         addresses = netifaces.ifaddresses(interface)[netifaces.AF_INET]
        
#         # Print the interface name and IP addresses
#         print(f"Interface: {interface}")
#         for address_info in addresses:
#             ip_address = address_info['addr']
#             print(f"IP Address: {ip_address}")
#         print()

# # Define the multicast group information
# multicast_group = '224.5.23.2'  # Multicast group address
# multicast_port = 10006  # Port to listen on

# # Define the network interface information
# interface_address = '224.5.23.2'  # Interface IP address
# interface_name = 'wlp60s0'  # Interface name

# # Create a socket
# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# # Set the socket options to specify the interface
# sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
# sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
# sock.setsockopt(socket.SOL_SOCKET, socket.SO_BINDTODEVICE, bytes(interface_name, 'utf-8'))

# # Bind the socket to the multicast address and port
# sock.bind((multicast_group, multicast_port))

# # Join the multicast group on the specified interface
# membership = socket.inet_aton(interface_address) + socket.inet_aton(multicast_group)
# sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, membership)

# # Receive and process packets
# while True:
#     data, address = sock.recvfrom(1024)
#     print(f'Received packet from {address}: {data.decode()}')


import socket

# Define the multicast group and port
multicast_group = '224.5.23.2'
multicast_port = 10006

# Define the network interface information
interface_address = '192.168.1.18'
interface_name = 'wlp60s0'

# Create a UDP socket with multicast support
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

# Set the socket options to enable multicast and specify the network interface
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((multicast_group, multicast_port))
sock.setsockopt(socket.SOL_IP, socket.IP_MULTICAST_IF, socket.inet_aton(interface_address))
sock.setsockopt(socket.SOL_IP, socket.IP_ADD_MEMBERSHIP, socket.inet_aton(multicast_group) + socket.inet_aton(interface_address))

# Receive and process packets indefinitely
while True:
    data, sender = sock.recvfrom(1024)
    print(f"Received packet from {sender}: {data}")

# Close the socket (this won't be reached in this example)
sock.close()
