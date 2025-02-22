import { useState } from 'react'

function Planet({planetname, planetcenter, planetradius, longitude_rad, fillcolor}) {
    let cx = planetcenter + planetradius*Math.cos(longitude_rad);
    let cy = planetcenter + planetradius*Math.sin(longitude_rad);
    return (
        <>
        <circle className="planet" key={planetname} id={planetname} cx={cx} cy={cy} r="2" stroke="black" strokeWidth=".5" fill={fillcolor} />
        {/* <text x={cx} y={cy} 
        text-anchor="middle"
        dy="-0.4em"
        fontSize={8}
        // stroke="blue" 
        // stroke-width="1px"
        > {planetname.slice(-1,planetname.length)}
        </text> */}
        </>
    )
}

function Orbit({planetname, planetcenter, planetradius, strokecolor}){
    let orbitname = planetname+"_orbit";
    return(
        <circle className="orbit" id={orbitname} key={orbitname} cx={planetcenter} cy={planetcenter} r={planetradius} stroke={strokecolor} strokeWidth=".5" fill="none" />
    )
}

function Orbits({_planetcolors}){
    let orbit_array = [
        <Orbit key={"Neptune"} planetname={"Neptune"} planetcenter={50} planetradius={45}  strokecolor={_planetcolors[7]}/>,
        <Orbit key={"Uranus"} planetname={"Uranus"} planetcenter={50} planetradius={40}  strokecolor={_planetcolors[6]}/>,
        <Orbit key={"Saturn"} planetname={"Saturn"} planetcenter={50} planetradius={35}  strokecolor={_planetcolors[5]}/>,
        <Orbit key={"Jupiter"} planetname={"Jupiter"} planetcenter={50} planetradius={30} strokecolor={_planetcolors[4]}/>,
        <Orbit key={"Mars"} planetname={"Mars"} planetcenter={50} planetradius={25}  strokecolor={_planetcolors[3]}/>,
        <Orbit key={"Earth"} planetname={"Earth"} planetcenter={50} planetradius={20}  strokecolor={_planetcolors[2]}/>,
        <Orbit key={"Venus"} planetname={"Venus"} planetcenter={50} planetradius={15}  strokecolor={_planetcolors[1]}/>,
        <Orbit key={"Mercury"} planetname={"Mercury"} planetcenter={50} planetradius={10}  strokecolor={_planetcolors[0]}/>,
    ]
    return(
        orbit_array
    )
}

function Planets({_longitudes, _planetcolors}){
    let planet_array = [
        <Planet key={"Neptune"} planetname={"Neptune ♆"} planetcenter={50} planetradius={45} longitude_rad={_longitudes[7]} fillcolor={_planetcolors[7]}/>,
        <Planet key={"Uranus"} planetname={"Uranus ♅"} planetcenter={50} planetradius={40} longitude_rad={_longitudes[6]} fillcolor={_planetcolors[6]}/>,
        <Planet key={"Saturn"} planetname={"Saturn ♄"} planetcenter={50} planetradius={35} longitude_rad={_longitudes[5]} fillcolor={_planetcolors[5]}/>,
        <Planet key={"Jupiter"} planetname={"Jupiter ♃"} planetcenter={50} planetradius={30} longitude_rad={_longitudes[4]} fillcolor={_planetcolors[4]}/>,
        <Planet key={"Mars"} planetname={"Mars ♂"} planetcenter={50} planetradius={25} longitude_rad={_longitudes[3]} fillcolor={_planetcolors[3]}/>,
        <Planet key={"Earth"} planetname={"Earth ♁"} planetcenter={50} planetradius={20} longitude_rad={_longitudes[2]} fillcolor={_planetcolors[2]}/>,
        <Planet key={"Venus"} planetname={"Venus ♀"} planetcenter={50} planetradius={15} longitude_rad={_longitudes[1]} fillcolor={_planetcolors[1]}/>,
        <Planet key={"Mercury"} planetname={"Mercury ☿"} planetcenter={50} planetradius={10} longitude_rad={_longitudes[0]} fillcolor={_planetcolors[0]}/>,
    ]
    return(
        planet_array
    )
}

function Orrery({longitudes, planetcolors}) {
  
  return (
    <>
      <div class="orrery">
        <svg viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
            <Orbits _planetcolors={planetcolors["planet_colors"]}/>
            <circle className="planet" id="sun" cx="50" cy="50" r="5" stroke="black" strokeWidth=".5" fill={planetcolors.sun_color} />
            <Planets _longitudes={longitudes} _planetcolors={planetcolors.planet_colors}/>
        </svg>
      </div>
    </>
  )
}

export default Orrery
