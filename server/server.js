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

app.listen(port, () => {
	console.log(`Listening on port ${port}`)
})

app.get("/google", function(req, response) {
	const type = req.query.type
	let location
	const zip = req.query.zip ? req.query.zip : ''
	request(`https://maps.googleapis.com/maps/api/geocode/json?address=${zip}&key=${googleKey}`, { json: true },(err, resp, body) => {
		if (err) { return console.log(err) }
		if (req.query.coord) {
			location = req.query.coord
		} else if (req.query.zip) {
			const coords = body.results[0].geometry.location
			location = `${coords.lat},${coords.lng}`
		} else {
			location = req.query.ipcoord
		}
		let dist = !req.query.dist ? '4000' : req.query.dist
		let minprice = !req.query.minprice ? '' : `&minprice=${req.query.minprice}`
		let maxprice = !req.query.maxprice ? '' : `&maxprice=${req.query.maxprice}`
		request(`https://maps.googleapis.com/maps/api/place/nearbysearch/json?type=restaurant&key=${googleKey}&radius=${dist}&location=${location}&opennow${maxprice}${minprice}`, { json: true }, (err, resp, body) => {
			let places = {}
			places.results = {}
			for (let place in body.results) {
				places.results[body.results[place].name] = {
					"name": body.results[place].name,
					"address": body.results[place].vicinity
				}
			}
			response.send(places)
		})
	})
})

app.get("/ipinfo", function(req, response) {
	request(`http://ipinfo.io?token=${ipinfoKey}`, { json: true },(err, resp, body) => {
		if (err) { return console.log(err) }
		response.send(body)
	})
})

