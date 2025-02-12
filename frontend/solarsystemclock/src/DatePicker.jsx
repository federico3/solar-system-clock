import './DatePicker.css'

function DatePicker({dateSetter,speedSetter,speedModifier,currentDate,currentSpeed}) {
//   let currentSpeed = speedModifier(document.getElementById('orrery-speed').value)
  return (
    <>
      <div>
        <label for="orrery-time">Date:</label>
        <input
        type="date"
        id="orrery-time"
        name="orrery-time"
        min="-3000-01-01T00:00"
        max="3000-01-01T00:00"
        value={currentDate}
        // onInput={dateSetter}
        onChange={dateSetter}
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
            // onInput={speedSetter}
            onChange={speedSetter}
            value={currentSpeed}
            list="speedvalues"
        />

        <datalist id="speedvalues">
            <option value="0" label="1"></option>
            <option value="2" label="100"></option>
            <option value="4" label="10⁴"></option>
            <option value="6" label="10⁶"></option>
            <option value="8" label="10⁸"></option>
            <option value="10" label="10¹⁰"></option>
        </datalist>
      </div>
    </>
  )
}

export default DatePicker


