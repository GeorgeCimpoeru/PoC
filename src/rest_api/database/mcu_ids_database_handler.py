import sqlite3
import logging

logger = logging.getLogger(__name__)


class McuIdsDatabaseHandler:
    def __init__(self):
        self.conn = sqlite3.connect('database/mcu_ids.db')
        self.create_table()

    def create_table(self):
        cursor = self.conn.cursor()
        cursor.execute('''CREATE TABLE IF NOT EXISTS mcu_ids (
                          id INTEGER PRIMARY KEY,
                          id_mcu INTEGER,
                          id_ecu1 INTEGER,
                          id_ecu2 INTEGER,
                          id_ecu3 INTEGER,
                          id_ecu4 INTEGER)''')
        self.conn.commit()

    def save_mcu_id(self, id_mcu, id_ecu1, id_ecu2, id_ecu3, id_ecu4):
        cursor = self.conn.cursor()
        cursor.execute('''INSERT INTO mcu_ids (id_mcu, id_ecu1, id_ecu2, id_ecu3, id_ecu4)
                          VALUES (?, ?, ?, ?, ?)''',
                       (id_mcu, id_ecu1, id_ecu2, id_ecu3, id_ecu4))
        self.conn.commit()

    def read_all_mcu_ids(self):
        """
        Reads all records from the mcu_ids table and logs the data.
        """
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM mcu_ids')
        rows = cursor.fetchall()
        for row in rows:
            logger.info(f"DB Record: {row}")

    def close(self):
        self.conn.close()
