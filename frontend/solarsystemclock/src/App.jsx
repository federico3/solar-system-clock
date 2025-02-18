import React, { useEffect, useState } from "react";

// Bootstrap CSS
import "bootstrap/dist/css/bootstrap.min.css";
// Bootstrap Bundle JS
import "bootstrap/dist/js/bootstrap.bundle.js";

import './App.css'

import DatePicker from './DatePicker'
import Orrery from './Orrery'
import AlertWidget from './AlertWidget'

let ROOT_URL = "http://localhost:8180"
// ROOT_URL = ""

function App() {

  let _longitudes = []
  for (let i=0; i<8; i++){
    _longitudes.push(Math.PI/4*i);
  }
  // We will store the longitudes as the state.
  const [longitudes, setLongitudes] = useState({
    longitudes: _longitudes,
  });
  const [date, setDate] = useState({
    date: Date.now(),
  });
  const [speed, setSpeed] = useState({
    speed: 0
  });
  const [clockmode_live, setClockmode_live] = useState({
    mode_live: false
  });

  const [response_alert, setResponse_alert] = useState({
    backend_state: "OK",
    backend_message: "This is a message",
    error: "This is an error",
  });

  // When the date picker is updated, this is called
  async function dateSetter(){
    // The picker produces a string in the form YYYY-MM-DD
    let utcTime_string = document.getElementById('orrery-time').value;
    // Through some magic, Date turns that to a Unix time.
    let utcTime_ms = Date.parse(utcTime_string);
    console.log("Date: "+utcTime_string+" or in UNIX time "+utcTime_ms);
    if (!isNaN(utcTime_ms)){
      setDate({date: utcTime_string})
      // If the date is meaningful, we send it over.
      try{
        const response = await fetch(ROOT_URL+'/setDate/', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8',
          },
          body: "date="+utcTime_ms,
        })
        if (!response.ok) {
          throw new Error(`Response status: ${response.status}`);
        }
        setResponse_alert({backend_state: "OK", backend_message: "Sending date OK", error: ""})
      } catch (error) {
        console.error(error.message);
        setResponse_alert({backend_state: "Error", backend_message: "Error sending new date: ", error: error.message})
      }
    }
  }

  // Help function to have a log speed slider
  function speedModifier(speed){return 10**speed};
  function speedModifierInverter(speed){return Math.log10(speed)};

  // When the speed is updated, this is called
  async function speedSetter(){
    let speed = document.getElementById('orrery-speed').value
    let expspeed = speedModifier(speed)
    console.log("Speed: "+speed)
    console.log("ExpSpeed: "+expspeed)
    // Update the state in the GUI
    setSpeed({speed: speed})
    try{
      // Call the endpoint
      const response = await fetch(ROOT_URL+'/setSpeed/', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8',
        },
        body: "speed="+expspeed,
      })
      if (!response.ok) {
        throw new Error(`Response status: ${response.status}`);
      }
      setResponse_alert({backend_state: "OK", backend_message: "Sending speed OK", error: ""})
    } catch (error) {
      console.error(error.message);
      setResponse_alert({backend_state: "Error", backend_message: "Error sending new speed: ", error: error.message})
    }
  }

  // When the mode is updated, this is called
  async function modeSetter(mode_is_live){
    console.log("Mode: "+speed)
    // Update the state in the GUI
    setClockmode_live({mode_live: mode_is_live})
    try{
      // Call the endpoint
      const response = await fetch(ROOT_URL+'/setMode/', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8',
        },
        body: "mode="+((mode_is_live==true)?"LIVE":"PLAYBACK"),
      })
      if (!response.ok) {
        throw new Error(`Response status: ${response.status}`);
      }
      setResponse_alert({backend_state: "OK", backend_message: "Sending mode OK", error: ""})
    } catch (error) {
      console.error(error.message);
      setResponse_alert({backend_state: "Error", backend_message: "Error sending new mode: ", error: error.message})
    }
  }

  // This tries to fetch longitudes
  async function longitudesGetter(){
    console.log("Trying to get some longitudes!")
    try{
      const response = await fetch(ROOT_URL+'/longitudes/')
      if (!response.ok) {
        throw new Error(`Response status: ${response.status}`);
      }

      const lon_json = await response.json();
      console.log(lon_json);
      setLongitudes({longitudes: lon_json["longitudes"]})
      setResponse_alert({backend_state: "OK", backend_message: "Fetching longitudes OK", error: ""})
    } catch (error) {
      console.error(error.message);
      setResponse_alert({backend_state: "Error", backend_message: "Error fetching longitudes: ", error: error.message})
      let _longitudes = []
      let randoffset = Math.random()
      for (let i=0; i<8; i++){
        _longitudes.push(randoffset+Math.PI/4*i);
      }
      setLongitudes({longitudes: _longitudes})
    }
  }

  // This tries to fetch time
  async function dateGetter(){
    console.log("Trying to get clock date!")
    try{
      const response = await fetch(ROOT_URL+'/date/')
      if (!response.ok) {
        throw new Error(`Response status: ${response.status}`);
      }
      // The response returns a UNIX time
      const unixtime_ms = await response.text();
      console.log(unixtime_ms);
      const date = new Date(parseFloat(unixtime_ms));
      console.log("Received date "+date+ " or in unixtime "+unixtime_ms + " or in ISO "+date.toISOString().substring(0,10));
      setDate({date: date.toISOString().substring(0,10)})
      setResponse_alert({backend_state: "OK", backend_message: "Fetching date OK", error: ""})
    } catch (error) {
      console.error(error.message);
      setResponse_alert({backend_state: "Error", backend_message: "Error fetching date: ", error: error.message})
      // If the fetch does not work, just set the date to now
      // setDate({date: Date.now()})
    }
  }

  // This tries to fetch clock speed
  async function speedGetter(){
    console.log("Trying to get clock speed!")
    try{
      const response = await fetch(ROOT_URL+'/speed/')
      if (!response.ok) {
        throw new Error(`Response status: ${response.status}`);
      }
      // The response returns a UNIX time
      const expspeed = await response.text();
      const speed = speedModifierInverter(expspeed);
      console.log(expspeed, speed);
      setSpeed({speed: speed})
      setResponse_alert({backend_state: "OK", backend_message: "Fetching speed OK", error: ""})
    } catch (error) {
      console.error(error.message);
      setResponse_alert({backend_state: "Error", backend_message: "Error fetching clock speed: ", error: error.message})
      // If the fetch does not work, just set the speed to 1x
      // setSpeed({speed: 0})
    }
  }
  
  async function modeGetter(){
    console.log("Trying to get clock mode!")
    try{
      const response = await fetch(ROOT_URL+'/mode/')
      if (!response.ok) {
        throw new Error(`Response status: ${response.status}`);
      }
      // The response returns "LIVE" or "PLAYBACK"
      const _mode = await response.text();
      if (_mode == "LIVE"){
        setClockmode_live({mode_live: true});
        console.log("Clock mode is LIVE");
        setResponse_alert({backend_state: "OK", backend_message: "Mode is LIVE", error: ""})
      } else if (_mode == "PLAYBACK") {
        setClockmode_live({mode_live: false})
        setResponse_alert({backend_state: "OK", backend_message: "Mode is PLAYBACK", error: ""})
        console.log("Clock mode is PLAYBACK");
      } else {
        console.log("Mode "+ _mode+" is not recognized");
        setResponse_alert({backend_state: "warning", backend_message: "Mode "+_mode+ "is not recognized", error: ""})
      }
    } catch (error) {
      console.error(error.message);
      setResponse_alert({backend_state: "Error", backend_message: "Error fetching mode: ", error: error.message})
      // If the fetch does not work, just set the speed to 1x
      // setSpeed({speed: 0})
    }
  }

  // Register an effect to fetch longitudes, speeds, and dates every x seconds
  useEffect(() => {
    const intervalCall = setInterval(() => {
      longitudesGetter();
      dateGetter();
      speedGetter();
      modeGetter();
    }, 20000);
    return () => {
      // clean up
      clearInterval(intervalCall);
    };
  }, []);

  // Build the application
  return (
    <>
    <AlertWidget 
           status={response_alert.backend_state}
           message={response_alert.backend_message}
           error={response_alert.error}
    />
    <h1>Solar System Clock</h1>
    <Orrery longitudes={longitudes.longitudes}/>

    <DatePicker dateSetter={dateSetter} speedSetter={speedSetter} modeSetter={modeSetter} SpeedModifier={speedModifier} currentDate={date.date} currentSpeed={speed.speed} currentModeIsLive={clockmode_live.mode_live}/>
    </>
  )
}

export default App
