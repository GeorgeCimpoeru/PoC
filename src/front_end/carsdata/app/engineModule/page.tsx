import CurrentSpeedCard from '../components/engine_components/currentSpeedCard'
import EngineStateCard from '../components/engine_components/engineStateCard'
import FuelConsumptionCard from '../components/engine_components/fuelConsumptionCard'
import FuelUsedCard from '../components/engine_components/fuelUsedCard'
import PowerOutputCard from '../components/engine_components/powerOutputCard'
import SerialNumberCard from '../components/engine_components/serialNumberCard'
import StateOfRunningCard from '../components/engine_components/stateOfRunningCard'
import TorqueCard from '../components/engine_components/torqueCard'
import CombustionRange from '../components/engine_components/rangeCombustionCard'
import MediumSpeedCard from '../components/engine_components/mediumSpeedCard'

const EnginesModule = async () => {
    const res = await fetch(
        'https://pocapi.pythonanywhere.com/api?service=engine_module',
        { cache: 'no-store' }
    );
    const jsonData = await res.json();

    return (
        <div className="h-screen flex flex-col">
            <nav className="bg-blue-900 p-8 flex justify-between items-center">
                <div className="text-white text-2xl font-bold">Engine Module</div>
            </nav>
            <div>
                <div className=" flex items-center justify-center">
                    <table>
                        <tr>
                            <td>
                                <CurrentSpeedCard current_speed={jsonData.query_params.current_speed}></CurrentSpeedCard>
                            </td>
                            <td>
                                <EngineStateCard engine_state={jsonData.query_params.engine_state}></EngineStateCard>
                            </td>
                            <td>
                                <FuelConsumptionCard fuel_consumption={jsonData.query_params.fuel_consumption} ></FuelConsumptionCard>
                            </td>
                            <td>
                                <FuelUsedCard fuel_used={jsonData.query_params.fuel_used}></FuelUsedCard>      
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <PowerOutputCard power_output={jsonData.query_params.power_output}></PowerOutputCard>
                            </td>
                            <td>
                                <SerialNumberCard serial_number={jsonData.query_params.serial_number}></SerialNumberCard>
                            </td>
                            <td>
                                <StateOfRunningCard state_of_running={jsonData.query_params.state_of_running}></StateOfRunningCard>
                            </td>
                            <td>
                                <TorqueCard torque={jsonData.query_params.torque}></TorqueCard>         
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <CombustionRange combustion_range={jsonData.query_params.torque}></CombustionRange>
                            </td>
                            <td>
                                <MediumSpeedCard medium_speed={jsonData.query_params.torque}></MediumSpeedCard>
                            </td>
                        </tr>

                    </table>
                </div>
            </div >
        </div >
    )

}

export default EnginesModule