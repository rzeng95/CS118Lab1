# CS118Lab1
CS118 Fall '15 Lab 1: Concurrent Web Server using BSD Sockets

Brendan Sio 804150223  
Roland Zeng 204150508

Using given skeleton code, we created a web server that successfully outputs .html, .gif, and .jpg files on localhost. All our changes are done in the "dostuff" function in serverFork.c

### How to run:

Add files you wish to open (.html, .gif, .jpg) into the same directory. We have added 'index.html' as an example.

In console type:
```
make
./serverFork 3000
```
This creates a connection to localhost. Open up a web browser and type 
```
Localhost:3000/index.html
```
```
Localhost:3000/test.gif
```
```
Localhost:3000/pic.jpg
```
You should see the file rendered onto the browser!
