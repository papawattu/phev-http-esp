import './style'
import { Component } from 'preact'
import Battery from './battery'

export default class App extends Component {


	render() {
		const AirCon = () => {
			const clickHandler = () => {
				fetch('/operation', {
					method : 'POST',
					headers : {
						'Accept' : 'application/json',
						'Content-Type' : 'application/json'
					},
					body : JSON.stringify({operation : { aircon : 'on'}})
				})
			}
			return <button onClick={clickHandler}>Aircon</button>
		}
		
		const data = { 
			battery : {
				subscribe : () => ({ battery : { soc : 70}})
			}
			
		}
		return (
			<div>
				<Battery data={data}></Battery><AirCon></AirCon>		
			</div>
		)
	}
}
