const config = require('../source/secret_config.json')
const {google} = require('googleapis')
const express = require('express')
const bodyParser = require('body-parser')

const app = express()
const port = 5000

app.use(bodyParser.urlencoded({ extended: true }))

const request = require('request')
const http = require('http').Server(app)
const io = require('socket.io')(http)

const googleKey = config.GOOGLE_API_KEY
const ipinfoKey = config.IPINFO_API_KEY

app.use(bodyParser.urlencoded({extended : true}));
app.get("/google", function(req, response) {
	request(`https://maps.googleapis.com/maps/api/place/findplacefromtext/json?input=Gatech&inputtype=textquery&fields=formatted_address,place_id,name,permanently_closed,types&key=${googleKey}`, { json: true },(err, resp, body) => {
		if (err) { return console.log(err) }
		response.send(body)
	})
})

app.get("/ipinfo", function(req, response) {
	request(`http://ipinfo.io?token=${ipinfoKey}`, { json: true },(err, resp, body) => {
		if (err) { return console.log(err) }
		response.send(body)
	})
})

app.listen(port, () => {
	console.log(`Listening on port ${port}`)
})
