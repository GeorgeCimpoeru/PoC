import sqlite3


class CanFrameDatabaseHandler:
    def __init__(self):
        self.conn = sqlite3.connect('database/can_frames.db')
        self.create_table()

    def create_table(self):
        cursor = self.conn.cursor()
        cursor.execute('''CREATE TABLE IF NOT EXISTS can_frames (
                          id INTEGER PRIMARY KEY,
                          service_name TEXT,
                          arbitration_id INTEGER,
                          data BLOB)''')
        self.conn.commit()

    def save_frame(self, service_name, arbitration_id, data):
        cursor = self.conn.cursor()
        cursor.execute('''INSERT INTO can_frames (service_name, arbitration_id, data)
                          VALUES (?, ?, ?)''',
                       (service_name, arbitration_id, data))
        self.conn.commit()

    def get_last_frame_for_service(self, service_name):
        cursor = self.conn.cursor()
        cursor.execute('''SELECT arbitration_id, data FROM can_frames
                          WHERE service_name = ? ORDER BY id DESC LIMIT 1''',
                       (service_name,))
        row = cursor.fetchone()
        return row if row else (None, None)

    def close(self):
        self.conn.close()
