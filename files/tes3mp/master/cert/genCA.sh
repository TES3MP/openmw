openssl genrsa -out rootCA.key 2048
openssl req -x509 -new -key rootCA.key -days 365 -out rootCA.crt

