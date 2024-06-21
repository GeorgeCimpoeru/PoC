import sqlite3

class CanFrameDatabaseHandler:
    def __init__(self, db_name="can_frames.db"):
        self.conn = sqlite3.connect(db_name)
        self.create_tables()

    def create_tables(self):
        cursor = self.conn.cursor()
        cursor.execute('''CREATE TABLE IF NOT EXISTS frames (
                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                          service_name TEXT,
                          arbitration_id INTEGER,
                          data TEXT)''')
        self.conn.commit()

    def save_frame(self, service_name, arbitration_id, data):
        data_str = ','.join([f"0x{x:02X}" for x in data])  # Convert list to comma-separated string
        cursor = self.conn.cursor()
        cursor.execute('''INSERT INTO frames (service_name, arbitration_id, data)
                          VALUES (?, ?, ?)''', (service_name, arbitration_id, data_str))
        self.conn.commit()

    def get_last_frame_for_service(self, service_name):
        cursor = self.conn.cursor()
        cursor.execute('''SELECT arbitration_id, data FROM frames
                          WHERE service_name = ?
                          ORDER BY id DESC LIMIT 1''', (service_name,))
        row = cursor.fetchone()
        if row:
            arbitration_id, data_str = row
            data = [int(x, 16) for x in data_str.split(',')]
            return arbitration_id, data
        return None, None

    def close(self):
        self.conn.close()
