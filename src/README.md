# SSL client and server for TCP

## build

```bash
gcc client.c -lssl -lcrypto -o client
gcc server.c -lssl -lcrypto -o server
```

## Generate a self-signed certificate

```bash
openssl genrsa 2048 > server.key
openssl req -new -key server.key > server.csr
openssl x509 -req -days 3650 -signkey server.key < server.csr > server.crt
```

## Run

- server
```bash
./server
```
- client
```bash
./client hostname message
```
