import './style'
import { Component } from 'preact'
import Config from './components/config'


const API_PREFIX = '/api'

const Status = props => {
	const statusClick = () => {
		fetch(API_PREFIX + '/status', {
			method : 'GET',
			headers : {
				'Accept' : 'application/json',
			}
		}).then(response => response.json())
			.then(json => props.changed(json.status.battery.soc))
	}
	return <div><button onClick={statusClick.bind(this)}>Status</button></div>
}

export default class App extends Component {

	constructor() {
		super()
		this.setState({ soc : undefined })
		this.configUpdate = config => {
			console.log(`SSID ${config.wifi.ssid} Password ${config.wifi.password} Host ${config.host} Port ${config.port}`)
			fetch(API_PREFIX + '/config', {
				method : 'POST',
				headers : {
					'Accept' : 'application/json',
					'Content-Type' : 'application/json'
				},
				body : JSON.stringify({ carConnection : { ssid : config.wifi.ssid, password : config.wifi.password, host : config.host, port : config.port}})
			}).then(response => (response.ok ? alert("Connected to Wifi") : alert("Error")))
		}
		this.sendRegister = () => {
			console.log('Register')
			fetch(API_PREFIX + '/registration', {
				method : 'POST',
				headers : {
					'Accept' : 'application/json',
					'Content-Type' : 'application/json'
				}
			}).then(response => (response.ok ? alert("Registered") : alert("Error registering")))
		}
	}
	render() {
		const AirCon = () => {
			const clickHandler = () => {
				fetch(API_PREFIX + '/operation', {
					method : 'POST',
					headers : {
						'Accept' : 'application/json',
						'Content-Type' : 'application/json'
					},
					body : JSON.stringify({ requests : [ {operation : { airCon : 'on'}} ] })
				}).then(response => (response.ok ? alert("Aircon on") : alert("Air con error")))
			}
			return <button onClick={clickHandler}>Air Conditioning On</button>
		}
		const HeadLights = () => {
			const clickHandler = () => {
				fetch(API_PREFIX + '/operation', {
					method : 'POST',
					headers : {
						'Accept' : 'application/json',
						'Content-Type' : 'application/json'
					},
					body : JSON.stringify({ requests : [ {operation : { headLights : 'on'}} ] })
				}).then(response => (response.ok ? alert("Head lights on") : alert("head lights error")))
			}
			return <button onClick={clickHandler}>Head Lights On</button>
		}
		const config = { config : { carConnection : { ssid: 'REMOTE45cfsc', password : 'fhcm852767', host : '192.168.8.46', port : 8080} } }	
		//Status.bind(this)
		const connect = () => 
			fetch(API_PREFIX + '/connect', {
				method : 'POST',
				headers : {
					'Accept' : 'application/json',
					'Content-Type' : 'application/json'
				}
			}).then(response => (response.ok ? alert("Connected") : alert("Connecting error")))
		const changed = soc => this.setState({ soc: soc })
		return (
			<div>
				<Config config={config} configUpdate={this.configUpdate} sendRegister={this.sendRegister}></Config>
				<button onClick={connect}>ConnectXXX</button>
				<AirCon></AirCon><Status soc={this.state.soc} changed={changed.bind(this)}></Status><div>Battery {this.state.soc}</div>
				<HeadLights></HeadLights>
			</div>
		)
	}
}
