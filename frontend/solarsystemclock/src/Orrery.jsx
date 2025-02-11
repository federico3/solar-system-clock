import { useState } from 'react'

function Planet({planetname, planetcenter, planetradius, longitude_rad, fillcolor}) {
    let cx = planetcenter + planetradius*Math.cos(longitude_rad);
    let cy = planetcenter + planetradius*Math.sin(longitude_rad);
    return (
        <circle className="planet" key={planetname} id={planetname} cx={cx} cy={cy} r="2" stroke="black" strokeWidth=".5" fill={fillcolor} />
    )
}

function Orbit({planetname, planetcenter, planetradius, strokecolor}){
    let orbitname = planetname+"_orbit";
    return(
        <circle className="orbit" id={orbitname} key={orbitname} cx={planetcenter} cy={planetcenter} r={planetradius} stroke={strokecolor} strokeWidth=".5" fill="none" />
    )
}

function Orbits(){
    let orbit_array = [
        <Orbit key={"Neptune"} planetname={"Neptune"} planetcenter={50} planetradius={45}  strokecolor={"#789EBF"}/>,
        <Orbit key={"Uranus"} planetname={"Uranus"} planetcenter={50} planetradius={40}  strokecolor={"#CEF2F2"}/>,
        <Orbit key={"Saturn"} planetname={"Saturn"} planetcenter={50} planetradius={35}  strokecolor={"#F2CD88"}/>,
        <Orbit key={"Jupiter"} planetname={"Jupiter"} planetcenter={50} planetradius={30} strokecolor={"#BF8136"}/>,
        <Orbit key={"Mars"} planetname={"Mars"} planetcenter={50} planetradius={25}  strokecolor={"#F27A5E"}/>,
        <Orbit key={"Earth"} planetname={"Earth"} planetcenter={50} planetradius={20}  strokecolor={"#BACBD9"}/>,
        <Orbit key={"Venus"} planetname={"Venus"} planetcenter={50} planetradius={15}  strokecolor={"#D9B391"}/>,
        <Orbit key={"Mercury"} planetname={"Mercury"} planetcenter={50} planetradius={10}  strokecolor={"#BFBEBD"}/>,
    ]
    return(
        orbit_array
    )
}

function Planets({_longitudes}){
    let planet_array = [
        <Planet key={"Neptune"} planetname={"Neptune"} planetcenter={50} planetradius={45} longitude_rad={_longitudes[7]} fillcolor={"#789EBF"}/>,
        <Planet key={"Uranus"} planetname={"Uranus"} planetcenter={50} planetradius={40} longitude_rad={_longitudes[6]} fillcolor={"#CEF2F2"}/>,
        <Planet key={"Saturn"} planetname={"Saturn"} planetcenter={50} planetradius={35} longitude_rad={_longitudes[5]} fillcolor={"#F2CD88"}/>,
        <Planet key={"Jupiter"} planetname={"Jupiter"} planetcenter={50} planetradius={30} longitude_rad={_longitudes[4]} fillcolor={"#BF8136"}/>,
        <Planet key={"Mars"} planetname={"Mars"} planetcenter={50} planetradius={25} longitude_rad={_longitudes[3]} fillcolor={"#F27A5E"}/>,
        <Planet key={"Earth"} planetname={"Earth"} planetcenter={50} planetradius={20} longitude_rad={_longitudes[2]} fillcolor={"#BACBD9"}/>,
        <Planet key={"Venus"} planetname={"Venus"} planetcenter={50} planetradius={15} longitude_rad={_longitudes[1]} fillcolor={"#D9B391"}/>,
        <Planet key={"Mercury"} planetname={"Mercury"} planetcenter={50} planetradius={10} longitude_rad={_longitudes[0]} fillcolor={"#BFBEBD"}/>,
    ]
    return(
        planet_array
    )
}

function Orrery({longitudes}) {
  
  return (
    <>
      <div>
        <svg viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
            <Orbits/>
            <circle className="planet" id="sun" cx="50" cy="50" r="5" stroke="black" strokeWidth=".5" fill="#f28322" />
            <Planets _longitudes={longitudes}/>
        </svg>
      </div>
    </>
  )
}

export default Orrery
