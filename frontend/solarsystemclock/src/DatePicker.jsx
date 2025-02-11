import { useState } from 'react'

function DatePicker({dateSetter,speedSetter,speedModifier}) {
  const [count, setCount] = useState(0)
//   let currentSpeed = speedModifier(document.getElementById('orrery-speed').value)
  return (
    <>
      <div>
        <label for="orrery-time">Choose a time:</label>
        <input
        type="date"
        id="orrery-time"
        name="orrery-time"
        min="-3000-01-01T00:00"
        max="3000-01-01T00:00"
        onInput={dateSetter}
        />
      </div>
      <div>
        <label for="orrery-speed">Speed:</label>
        <input
        type="range"
        id="orrery-speed"
        name="orrery-speed"
        min="0"
        max="10"
        step="0.1"
        onInput={speedSetter}
       />
      </div>
    </>
  )
}

export default DatePicker


