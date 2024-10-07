import type { NextApiRequest, NextApiResponse } from 'next';
import {Database} from 'sqlite3';

export default async function handler(req: NextApiRequest, res: NextApiResponse) {
    const { unit } = req.query;
    if (!unit) {
        return res.status(400).json({ error: 'Unit is required' });
    }

    /* Ensure unit is a string */
    const unitString = Array.isArray(unit) ? unit[0] : unit;

    /* Method that executes the needed querry */
    function getAvailableForModule(unit:string) {
        const db = new Database('ota.db');

        /* Do the Select and close the database */
        return new Promise((resolve, reject) => {
            db.get(`SELECT versions FROM available_versions WHERE unit = '${unit}'`,(err:any, results:any) => {
                db.close();
                if (err) {
                    reject(err);
                }
                    
                resolve(results || null);
            });
        });
    }

    try {
        const versions = await getAvailableForModule(unitString);
        res.status(200).json({ versions });
    } catch (error) {
        res.status(500).json({ error: 'Versions could not be retrieved' });
    }
}