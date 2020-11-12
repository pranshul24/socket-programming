# OS Assignment-6

## To run the code

### First go to the directory in which **server.c** is present and then run the server as :
```
$ gcc server.c
$ ./a.out
```
### Then go to the directory in which **client.c** is present and then run client as :
```
$ gcc client.c
$ ./a.out
```
---

## Layout

### Server 

- Prints the files that are requested
- If the file is present in the server directory then sends it else prints proper message
- When file is sent then prints accordingly 
- Use `Ctrl+C` to exit the server

### Client 

- The client side is CLI
- Completely tokenized (spaces and tabs do not matter)
- `get <file>` is used to download the requested file 
- `get <file1> <file2> ...` is used to download multiple files (one after the other)
- If file is not present (or permission to read is not there) then message is printed
- If file is present then file is downloaded while the percentage and the progress bar is printed
- `exit` is used to exit the client **( Do not use Ctrl+C to exit client )**
- any other command does not affect 

---