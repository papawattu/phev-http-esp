import { Component } from 'preact'

const EditInput = props => <div><label for={props.name}>{props.name}</label><input name={props.name} value={props.value} onChange={props.onChange}/></div>

const Wifi = props => <div><EditInput name="ssid" value={props.ssid} onChange={props.onChange}></EditInput><EditInput name="password" value={props.password} onChange={props.onChange}></EditInput></div>

const Host = props => <div><EditInput name="host" value={props.host} onChange={props.onChange}></EditInput><EditInput name="port" value={props.port} onChange={props.onChange}></EditInput></div>

export default class Config extends Component {
    constructor(props) {
        super(props)

        const { carConnection } = props.config.config
        
        this.setState({wifi :{ ssid: carConnection.ssid, password: carConnection.password}, host : carConnection.host, port : carConnection.port})
        this.change = this.change.bind(this)
        this.update = this.update.bind(this)
        this.register = this.register.bind(this)
        this.configUpdate = props.configUpdate
        this.sendRegister = props.sendRegister
    }
    
    change(e) {
        if(e.target.name === 'ssid') {
            this.setState({ wifi : { ssid : e.target.value, password: this.state.wifi.password}, host :this.state.host, port :this.state.port})
        }
        if(e.target.name === 'password') {
            this.setState({ wifi : { ssid : this.state.wifi.ssid, password: e.target.value}, host :this.state.host, port :this.state.port})
        }
        if(e.target.name === 'host') {
            this.setState({ wifi : { ssid : this.state.wifi.ssid, password: this.state.wifi.password}, host: e.target.value, port : this.state.port})
        }
        if(e.target.name === 'port') {
            this.setState({ wifi : { ssid : this.state.wifi.ssid, password: this.state.wifi.password}, host: this.state.host, port : e.target.value})
        }


    }
    update() {
        this.configUpdate(this.state)
    }
    register() {
        this.sendRegister()
    }
    render() {
        
        return (
            <div>
                <Wifi ssid={this.state.wifi.ssid} password={this.state.wifi.password} onChange={this.change}></Wifi>
                <Host host={this.state.host} port={this.state.port} onChange={this.change}></Host>
                <button onClick={this.update}>Wifi</button><button onClick={this.register}>Register</button>
            </div>
        )
    }
}