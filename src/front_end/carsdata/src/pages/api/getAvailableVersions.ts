import type { NextApiRequest, NextApiResponse } from 'next';
import {Database} from 'sqlite3';

export default async function handler(req: NextApiRequest, res: NextApiResponse) {

    function getAllAvailable() {
        const db = new Database('ota.db');

        /* Do the Select and close the database */
        return new Promise((resolve, reject) => {
            db.all(`SELECT versions FROM available_versions`,(err:any, results:any) => {
                db.close();
                if (err) {
                    reject(err);
                }
                resolve(results || null);
            });
        });
    }

    try {
        const versions = await getAllAvailable();
        res.status(200).json({ versions });
    } catch (error) {
        res.status(500).json({ error: 'Versions could not be retrieved' });
    }
}