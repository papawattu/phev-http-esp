import './style'
import { Component } from 'preact'
import Config from './components/config'

const Status = props => {
	const statusClick = () => {
		fetch('/status', {
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
		this.wifiUpdate = wifi => {
			console.log(`SSID ${wifi.ssid} Password ${wifi.password}`)
			fetch('/config', {
				method : 'POST',
				headers : {
					'Accept' : 'application/json',
					'Content-Type' : 'application/json'
				},
				body : JSON.stringify({ carConnection : { ssid : wifi.ssid, password : wifi.password}})
			}).then(response => (response.ok ? alert("Hello") : alert("Error")))
		}
		this.sendRegister = () => {
			console.log('Register')
			fetch('/register', {
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
				fetch('/operation', {
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
		
		const config = { config : { carConnection : { ssid: 'REMOTE45cfsc', password : 'fhcm852767'} } }	
		//Status.bind(this)
		const connect = () => 
			fetch('/connect', {
				method : 'POST',
				headers : {
					'Accept' : 'application/json',
					'Content-Type' : 'application/json'
				}
			}).then(response => (response.ok ? alert("Connected") : alert("Connecting error")))
		const changed = soc => this.setState({ soc: soc })
		return (
			<div>
				<Config config={config} wifiUpdate={this.wifiUpdate} sendRegister={this.sendRegister}></Config>
				<button onClick={connect}>Connect</button>
				<AirCon></AirCon><Status soc={this.state.soc} changed={changed.bind(this)}></Status><div>Battery {this.state.soc}</div>
			</div>
		)
	}
}
