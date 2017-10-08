openssl genrsa -des3 -out master.des3.key 4096
openssl rsa -in master.des3.key -out master.key
openssl req -new -key master.key -out master.csr -reqexts SAN -config master.conf

openssl x509 -req -days 365 -in master.csr -CA rootCA.crt -CAkey rootCA.key -CAcreateserial -out master.crt -extensions SAN -extfile master.conf


openssl pkcs12 -export -clcerts -in master.crt -inkey master.key -out master.p12
openssl pkcs12 -in master.p12 -out master.pem -clcerts
