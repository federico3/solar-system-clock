import React, { useEffect, useState } from "react";
import './App.css'
import DatePicker from './DatePicker'
import Orrery from './Orrery'

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

  // When the date picker is updated, this is called
  async function dateSetter(){
    let utcTime_string = document.getElementById('orrery-time').value;
    let utcTime_ms = Date.parse(utcTime_string);
    console.log("Date: "+utcTime_string+" or in UNIX time "+utcTime_ms);
    if (!isNaN(utcTime_ms)){
      setDate({date: utcTime_string})
      try{
        const response = await fetch('/setDate/', {
          method: 'POST',
          headers: {
            'Accept': 'application/json',
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            unixDate_ms: utcTime_ms,
          })
        })
        if (!response.ok) {
          console.error(`Response status: ${response.status}`);
        }
      } catch (error) {
        console.error(error.message);
      }
    }
  }

  // Help function to have a log speed slider
  function speedModifier(speed){return 10**speed};
  // When the speed is updated, this is called
  async function speedSetter(){
    let speed = document.getElementById('orrery-speed').value
    let expspeed = speedModifier(speed)
    console.log("Speed: "+speed)
    console.log("ExpSpeed: "+expspeed)
    setSpeed({speed: speed})
    try{
      const response = await fetch('/setSpeed/', {
        method: 'POST',
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          speed: expspeed,
        })
      })
      if (!response.ok) {
        throw new Error(`Response status: ${response.status}`);
      }
    } catch (error) {
      console.error(error.message);
    }
  }

  // This tries to fetch longitudes
  async function longitudesGetter(){
    console.log("Trying to get some longitudes!")
    try{
      const response = await fetch('/longitudes/')
      if (!response.ok) {
        throw new Error(`Response status: ${response.status}`);
      }

      const lon_json = await response.json();
      console.log(lon_json);
      setLongitudes({longitudes: lon_json["longitudes"]})
    } catch (error) {
      console.error(error.message);
      let _longitudes = []
      let randoffset = Math.random()
      for (let i=0; i<8; i++){
        _longitudes.push(randoffset+Math.PI/4*i);
      }
      setLongitudes({longitudes: _longitudes})

    }
  }
  

  // Register an effect to fetch longitudes every x seconds
  useEffect(() => {
    const intervalCall = setInterval(() => {
      longitudesGetter();
    }, 10000);
    return () => {
      // clean up
      clearInterval(intervalCall);
    };
  }, []);

  // Build the application
  return (
    <>
    <h1>Solar System Clock</h1>
    <Orrery longitudes={longitudes.longitudes}/>

    <DatePicker dateSetter={dateSetter} speedSetter={speedSetter} SpeedModifier={speedModifier} currentDate={date.date} currentSpeed={speed.speed}/>
    </>
  )
}

export default App
