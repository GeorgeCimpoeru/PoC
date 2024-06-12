import can 

class GenerateFrame:
    def __init__(self, can_interface):
        self.bus = can.interface.Bus(channel=can_interface, bustype='socketcan')

    
    def send_frame(self, id, data):
        message = can.Message(arbitration_id=id, data=data)
        try: 
            self.bus.send(message)
        except can.CanError:
            print("Message not sent")

    def session_control(self, id, sub_funct):
        data = [2, 0x10, sub_funct]

        self.send_frame(id, data) 


can_interface = "vcan0"
id = 0x123
data = [0,1,2]

generateFrame = GenerateFrame(can_interface)

generateFrame.session_control(id, 0x3)

generateFrame.bus.shutdown()

