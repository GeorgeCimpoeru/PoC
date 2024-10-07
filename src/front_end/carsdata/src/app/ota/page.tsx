import OTApage from "@/src/components/OTAcomponents/OTApage";
import {Database} from 'sqlite3';

let modules = ["HVAC", "Doors", "Engine", "Battery", "MCU"];
let modules_ids = [1, 2, 3, 7, 8];

const OTA = () => {

    /* Get an instance of OTA database */
    const db = new Database('ota.db');

    const getNewSoftVersions = async () => {
        console.log("Getting new soft versions...");
        try {
            await fetch(`http://127.0.0.1:5000/api/drive_update_data`, {
                method: 'GET',
                mode: 'cors',
            }).then(response => response.json())
                .then(data => {
                    console.log(data);

                    db.exec("UPDATE available_versions SET versions=''");
                     
                    /* Iterate trough each module */
                    for (let i = 0; i < modules.length; ++i) {
                        let module_name = modules[i];
                        let module_id = modules_ids[i];

                        let versions = '';

                        /* Increment trough each available version and build the array */
                        for (let j = 0; j < data.children[module_id].children.length; ++j) {
                            versions = versions + data.children[module_id].children[j].sw_version + '; ';
                        }
                        versions = versions.slice(0, -2);

                        /* Update the database */
                        const sql = db.prepare('UPDATE available_versions SET versions = ? WHERE unit = ?');
                        sql.run([versions, module_name]);
                    }
                });
        } catch (error) {
            console.log(error);
        } finally {
            db.close();
            console.log("Database updated with new soft versions...");
        }
    };

    getNewSoftVersions();

    return (
        <OTApage />
    )
}

export default OTA
