var bodyParser = require("body-parser");
const express = require('express');

const app = express();

var http = require('http');
var path = require("path");

app.use(bodyParser.urlencoded({
    extended: false
}));

app.use(bodyParser.json());

const server = http.createServer(app); //create a server

require('dns').lookup(require('os').hostname(), function (err, add, fam) {
    console.log('addr: ' + add);
})

//websocket setup

const WebSocket = require('ws');
const s = new WebSocket.Server({
    server
});

//viewed at http://localhost:3000

app.get('/', function (req, res) {
    res.sendFile(path.join(__dirname + '/index.html'));
});

s.on('connection', function (ws, req) {

/******* when server receives messsage from client trigger function with argument message *****/

    ws.on('message', function (message) {

        console.log("Received: " + message);
        s.clients.forEach(function (client) {

            if (client != ws && client.readyState) {
                client.send("broadcast: " + message);
            }

        });
    });

    ws.on('close', function () {
        console.log("lost one client");
    });
    console.log("new client connected");
});

server.listen(3000);