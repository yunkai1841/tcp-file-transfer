# TCP-messenger
Send a single message to a server use TCP.

## Build
```bash
make all
```
or
```bash
cd src
gcc -o server server.c
gcc -o client client.c
```

## Usage
Server
```bash
cd src
./server
```

Client
```bash
cd src
./client <server hostname or ip> <message>
```


## 参考資料
https://daeudaeu.com/c-http-server/
https://9cguide.appspot.com/17-02.html
https://www.ibm.com/docs/ja/zos/2.3.0?topic=functions-recv
https://zenn.dev/blackengineer/articles/105a94783a205612f548
