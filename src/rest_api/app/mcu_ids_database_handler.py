import sqlite3

class McuIdsDatabaseHandler:
    def __init__(self, db_name="mcu_ids.db"):
        self.conn = sqlite3.connect(db_name)
        self.create_tables()

    def create_tables(self):
        cursor = self.conn.cursor()
        cursor.execute('''CREATE TABLE IF NOT EXISTS mcu_ids (
                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                          arbitration_id INTEGER,
                          pcl INTEGER,
                          sid INTEGER,
                          id_mcu INTEGER,
                          id_bat INTEGER,
                          id_dos INTEGER,
                          id_eng INTEGER)''')
        self.conn.commit()

    def save_mcu_id(self, arbitration_id, pcl, sid, id_mcu, id_bat, id_dos, id_eng):
        cursor = self.conn.cursor()
        cursor.execute('''INSERT INTO mcu_ids (arbitration_id, pcl, sid, id_mcu, id_bat, id_dos, id_eng)
                          VALUES (?, ?, ?, ?, ?, ?, ?)''', (arbitration_id, pcl, sid, id_mcu, id_bat, id_dos, id_eng))
        self.conn.commit()

    def close(self):
        self.conn.close()
