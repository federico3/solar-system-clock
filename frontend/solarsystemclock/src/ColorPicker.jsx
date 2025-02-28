import { useState } from 'react'
import { ChromePicker, BlockPicker } from 'react-color';

function PlanetColor({planetname, fillcolor, colorChanger}) {

    return (
        <div className="col-sm-3 planetcolor">
            <h5>{planetname}</h5>
            <ChromePicker disableAlpha color={fillcolor} onChangeComplete={colorChanger}/>
        </div>
    )
}

function PlanetColors({_planetcolors, _planetColorChangers}){
    let planet_array = [
        <PlanetColor key={"Neptune ♆"} planetname={"Neptune ♆"} fillcolor={_planetcolors[7]} colorChanger={(color)=>{_planetColorChangers(7,color)}}/>,
        <PlanetColor key={"Uranus ♅"} planetname={"Uranus ♅"}   fillcolor={_planetcolors[6]} colorChanger={(color)=>{_planetColorChangers(6,color)}}/>,
        <PlanetColor key={"Saturn ♄"} planetname={"Saturn ♄"}   fillcolor={_planetcolors[5]} colorChanger={(color)=>{_planetColorChangers(5,color)}}/>,
        <PlanetColor key={"Jupiter ♃"} planetname={"Jupiter ♃"} fillcolor={_planetcolors[4]} colorChanger={(color)=>{_planetColorChangers(4,color)}}/>,
        <PlanetColor key={"Mars ♂"} planetname={"Mars ♂"}       fillcolor={_planetcolors[3]} colorChanger={(color)=>{_planetColorChangers(3,color)}}/>,
        <PlanetColor key={"Earth ♁"} planetname={"Earth ♁"}     fillcolor={_planetcolors[2]} colorChanger={(color)=>{_planetColorChangers(2,color)}}/>,
        <PlanetColor key={"Venus ♀"} planetname={"Venus ♀"}     fillcolor={_planetcolors[1]} colorChanger={(color)=>{_planetColorChangers(1,color)}}/>,
        <PlanetColor key={"Mercury ☿"} planetname={"Mercury ☿"} fillcolor={_planetcolors[0]} colorChanger={(color)=>{_planetColorChangers(0,color)}}/>,
    ]
    return(
        planet_array.reverse()
    )
}

function PlanetColorPicker({planetcolors, colorchangers}) {
  const [colorsVisible, setColorsVisible] = useState({
    visible: false,
  });
  function ChangeColorVisibility(){
    colorsVisible.visible==true?setColorsVisible({visible:false}):setColorsVisible({visible:true});
  }

  function sunColorChanger(newcolor){
    colorchangers({sun_color: newcolor.hex, planet_colors: planetcolors.planet_colors})
  }
  function planetColorChangers(planet_index, newcolor){
    let new_planet_colors = planetcolors.planet_colors
    new_planet_colors[planet_index] = newcolor.hex
    colorchangers({sun_color: planetcolors.sun_color, planet_colors: new_planet_colors})
  }

  let default_planetcolors_int = {"planet_colors":[12566205,14267281,12241881,15891038,12550454,15912328,13562610,7904959],"sun_color":15893282}
  let _planetcolors = {"planet_colors": ["","","","","","","",""], "sun_color": ""}
  _planetcolors["sun_color"] = "#"+default_planetcolors_int["sun_color"].toString(16).padStart(6, '0');
  for (let planet_index = 0; planet_index<_planetcolors["planet_colors"].length; planet_index++){
    _planetcolors["planet_colors"][planet_index]="#"+default_planetcolors_int["planet_colors"][planet_index].toString(16).padStart(6, '0');
  }
  
  function resetDefaultColors(){
    colorchangers({
        planet_colors: _planetcolors["planet_colors"],
        sun_color: _planetcolors["sun_color"]
      })
  }
  
  return (
    <>
     <button type="button" className="collapsible" onClick={ChangeColorVisibility}><h3>Colors {colorsVisible.visible==true?"-":"+"}</h3></button>
      <div className="row colorcontent" style={{display: colorsVisible.visible?"flex":"none"}}>
            <PlanetColor planetname={"Sun ☉︎"} fillcolor={planetcolors.sun_color} colorChanger={sunColorChanger}/>
            <PlanetColors _planetcolors={planetcolors.planet_colors} _planetColorChangers={planetColorChangers}/>
            <div className="col-sm-3" style={{paddingTop: "2em"}}>

                <button style={{padding: "10%"}} onClick={resetDefaultColors}>Reset all to defaults</button>
            </div>
      </div>
    </>
  )
}

export default PlanetColorPicker
