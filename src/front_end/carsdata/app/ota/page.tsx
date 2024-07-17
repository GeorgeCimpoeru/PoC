'use client';
import LeftSideBar from "../components/OTAcomponents/LeftSideBar";
import NavbarOta from "../components/OTAcomponents/NavbarOta";
import TableOta from "../components/OTAcomponents/TableOta";
import DeploymentHistory from "../components/OTAcomponents/DeploymentHistory";



const OTApage = () => {
    if (true) {
        return (
            <div className="flex w-full h-screen flex-col lg:flex-row">
                <LeftSideBar></LeftSideBar>
                <div className="w-[94%] h-screen flex flex-col">
                    <NavbarOta></NavbarOta>
                    {/* <button className="btn btn-ghost m-1 mt-1 mr-6 ml-6 justify-end">Deployment history</button> */}
                    <DeploymentHistory></DeploymentHistory>
                    <TableOta></TableOta>
                </div>
            </div>
        );

    }

    else {
        return (
            <div className="flex w-full h-screen flex-col lg:flex-row">
                <LeftSideBar></LeftSideBar>
                <div className="w-[94%] h-screen flex flex-col">
                    <NavbarOta></NavbarOta>
                </div>
            </div>
        );

    }
}

export default OTApage;

