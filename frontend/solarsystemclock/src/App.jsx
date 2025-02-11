import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'
import DatePicker from './DatePicker'
import Orrery from './Orrery'

function App() {
  const [count, setCount] = useState(0)

  function dateSetter(){console.log("Date: "+document.getElementById('orrery-time').value)}
  function speedModifier(speed){return 10**speed};
  function speedSetter(){
    console.log("Speed: "+document.getElementById('orrery-speed').value)
    console.log("ExpSpeed: "+speedModifier(document.getElementById('orrery-speed').value))
  }
  
  let longitudes = []
  for (let i=0; i<8; i++){
    longitudes.push(Math.PI/4*i);
  }

  return (
    <>
    <DatePicker dateSetter={dateSetter} speedSetter={speedSetter} SpeedModifier={speedModifier}/>
    <Orrery longitudes={longitudes}/>
    </>
  )
}

export default App
