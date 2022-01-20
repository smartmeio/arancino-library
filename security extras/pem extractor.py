#It is used to extract the encoded pem string that represents the certificates.
#The device certificate and the signer certificate are stored inside the certificates slot of cryptochip ATECC608B TrustFlex.
#The extract strings must be insert in the Arancino Library header file.

#It is used to extract the encoded pem string that represents the device public key.
#The extract string must be insert in the Redis database of Arancino Module containing device public keys.


from cryptography.x509 import load_pem_x509_certificate
from cryptography.hazmat.primitives.serialization import Encoding, PublicFormat
import os

cur_path = os.path.dirname(__file__)
add_path_device = '/DeviceCertificate.pem'
add_path_signer = '/SignerCertificate.pem'
new_path_device = cur_path + add_path_device
new_path_signer = cur_path + add_path_signer
file_device_cert = open(new_path_device, "rb")
device_data_cert = file_device_cert.read()
file_signer_cert = open(new_path_signer, "rb")
signer_data_cert = file_signer_cert.read()


device_cert_obj = load_pem_x509_certificate(device_data_cert)
device_cert_bytes = device_cert_obj.public_bytes(encoding=Encoding.PEM)
print("Device Cert: " + str(device_cert_bytes))
#print("".join(hex(n).upper() for n in device_cert_bytes))

device_cert_pubkey_obj = device_cert_obj.public_key()
device_cert_pubkey_bytes = device_cert_pubkey_obj.public_bytes(format=PublicFormat.SubjectPublicKeyInfo, encoding=Encoding.PEM)
print("Device Pub Key: " + str(device_cert_pubkey_bytes))
#print("".join(hex(n).upper() for n in device_cert_pubkey_bytes))

signer_cert_obj = load_pem_x509_certificate(signer_data_cert)
signer_cert_bytes = signer_cert_obj.public_bytes(encoding=Encoding.PEM)
print("Signer Cert: " + str(signer_cert_bytes))
#print("".join(hex(n).upper() for n in signer_cert_bytes))