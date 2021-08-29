require('dotenv').config();

var bodyParser = require("body-parser");
const express = require('express');
const nodemailer = require('nodemailer')

//Mongo stuff
const MongoClient = require('mongodb').MongoClient;
const assert = require('assert');
const uri = process.env.DATABASE_URL;


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

//Email Part
const transporter = nodemailer.createTransport({
    service: "gmail",
    auth: {
        user:   'greensgomail@gmail.com',
        pass:   process.env.PASSWORD
    }

    //Fix this to be invisible
})


//===============================================================================================

s.on('connection', function (ws, req) {

/******* when server receives messsage from client trigger function with argument message *****/

    ws.on('message', function (message) {
        
        //console.log("Received: " + message);

            let rawData = message.split(',');
            let temp = rawData[0];
            let humi = rawData[1];
            let mois = rawData[2];
            let light = rawData[3];

            // Data conversion according to calibration
            // V measuremnets from Arduino (Moisture Meter)
            // 0 => Dry as can be
            // -+ 350 => Dry soil
            // -+ 700 => Moist soil
            // -+ 780 => Extremly wet

            let moisPer = Math.round(((mois*100)/780))

            // V measuremnets from Arduino (Light Meter)
            // 0 => Dry as can be
            // -+ 50-200 => Indoor light
            // -+ 300-450 => Outside Shade light
            // -+ 800-960 => Outside Sun light

            let lightPer = Math.round(((light*100)/960))

            if (lightPer > 100) {
                lightPer = 100
            }

            if (moisPer > 100) {
                moisPer = 100
            }

            console.log("Temperature: "+ temp +"\nHumidity: "+ humi +"\nMoisture: "+ moisPer +"\nLight: "+ lightPer +"\n");
            //Send to mongoDB

            //Time instance
            let time = new Date(Date.now()).toLocaleString('en-ZA', { timeZone: 'Africa/Harare' });

            var ticker = new Date(Date.now())
            let _time = ticker.getHours() + ":" + ticker.getMinutes() + ":" + ticker.getSeconds();

            // condition == 6pm fix date part to only check for 6 pm
            if (_time == '18:00:00') {

                const mailOptions = {
                    from: 'greensgomail@gmail.com',
                    to: 'gustafdelport@gmail.com',
                    subject: 'Sensor Report',
                    text: `Time: ${time}\n\nTemperature: ${temp} Celsuis\nHumidity: ${temp}%\nMoisture: ${moisPer}%\nLight: ${lightPer}%`
                }
    
                transporter.sendMail(mailOptions,(err,info) => {
                    if (err) {
                        console.log(err);
                    } 
                    else {
                        console.log('Email sent: ' + info.response);
                    } 
                })
            }
            

            MongoClient.connect(uri, function(err,client) {
                assert.equal(null,err);
                const db = client.db("myFirstDatabase");

                db.collection('datas').insertOne({
                    //JSON obj
                    Time: time,
                    Temperature: temp,
                    HumidityPercentage: humi,
                    MoisturePercentage: moisPer,
                    LightIndex: lightPer
                })
                client.close();
            })
    });

    ws.on('close', function () {
        console.log("lost one client");
    });

    console.log("new client connected");
});

server.listen(3000);