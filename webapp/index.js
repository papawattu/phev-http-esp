import './style'
import { Component } from 'preact'
import Battery from './battery'

export default class App extends Component {


	render() {
		const Wifi = props => props.config().then(config => console.log(config.config.carConnection.ssid)) 
		const Config = props => {
			return <div>
				<Wifi config={props.config}></Wifi>
			</div>
		}
		const AirCon = () => {
			const clickHandler = () => {
				fetch('/operation', {
					method : 'POST',
					headers : {
						'Accept' : 'application/json',
						'Content-Type' : 'application/json'
					},
					body : JSON.stringify({operation : { aircon : 'on'}})
				}).then(response => (response.ok ? alert("Hello") : alert("Error")))
			}
			return <button onClick={clickHandler}>Aircon</button>
		}
		
		const data = { 
			battery : {
				subscribe : (cb) => {
					fetch('/status')
						.then(response => response.json())
						.then(json => cb(json))
				}
			}
			
		}
		const config = () =>
			fetch('/config')
						.then(response => response.json())
			
			
		return (
			<div>
				<Battery data={data}></Battery><AirCon></AirCon>		
				<Config config={config}></Config>
			</div>
		)
	}
}
