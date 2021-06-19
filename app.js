var bodyParser = require("body-parser");
const express = require('express'); //express framework to have a higher level of methods
const app = express(); //assign app variable the express class/method

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
/**********************websocket setup**************************************************************************************/
//var expressWs = require('express-ws')(app,server);

const WebSocket = require('ws');
const s = new WebSocket.Server({
    server
});

//when browser sends get request, send html file to browser
// viewed at http://localhost:3000

app.get('/', function (req, res) {
    res.sendFile(path.join(__dirname + '/index.html'));
});

//app.ws('/echo', function(ws, req) {
s.on('connection', function (ws, req) {

    /******* when server receives messsage from client trigger function with argument message *****/

    ws.on('message', function (message) {
        console.log("Received: " + message);
        s.clients.forEach(function (client) { //broadcast incoming message to all clients (s.clients)

            if (client != ws && client.readyState) { //except to the same client (ws) that sent this message
                client.send("broadcast: " + message);
            }
        });

        // ws.send("From Server only to sender: "+ message); //send to client where message is from
    });

    ws.on('close', function () {
        console.log("lost one client");
    });
    console.log("new client connected");
});

server.listen(3000);