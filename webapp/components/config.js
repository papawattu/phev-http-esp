import { Component } from 'preact'

const EditInput = props => <div><label for={props.name}>{props.name}</label><input name={props.name} value={props.value} onChange={props.onChange}/></div>

const Wifi = props => <div><EditInput name="ssid" value={props.ssid} onChange={props.onChange}></EditInput><EditInput name="password" value={props.password} onChange={props.onChange}></EditInput></div>

export default class Config extends Component {
    constructor(props) {
        super(props)

        const { carConnection } = props.config.config
        
        this.setState({wifi :{ ssid: carConnection.ssid, password: carConnection.password}})
        this.change = this.change.bind(this)
        this.update = this.update.bind(this)
        this.register = this.register.bind(this)
        this.wifiUpdate = props.wifiUpdate
        this.sendRegister = props.sendRegister
    }
    
    change(e) {
        if(e.target.name === 'ssid') {
            this.setState({ wifi : { ssid : e.target.value, password: this.state.wifi.password}})
        }
        if(e.target.name === 'password') {
            this.setState({ wifi : { ssid : this.state.wifi.ssid, password: e.target.value}})
        }
    }
    update() {
        this.wifiUpdate(this.state.wifi)
    }
    register() {
        this.sendRegister()
    }
    render() {
        
        return (
            <div>
                <Wifi ssid={this.state.wifi.ssid} password={this.state.wifi.password} onChange={this.change}></Wifi>
                <button onClick={this.update}>Wifi</button><button onClick={this.register}>Register</button>
            </div>
        )
    }
}