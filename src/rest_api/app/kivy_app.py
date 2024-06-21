import kivy
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.dropdown import DropDown
from kivy.uix.button import Button
from kivy.uix.label import Label
from kivy.uix.textinput import TextInput
from kivy.clock import Clock
from frame_requester import FrameRequester
import can
import json

class MainApp(App):
    def build(self):
        # Setup virtual CAN interface
        self.setup_vcan_interface()
        return MainLayout()

    def setup_vcan_interface(self):
        try:
            bus = can.interface.Bus(channel='vcan1', interface='socketcan')
        except OSError:
            import os
            os.system('sudo modprobe vcan')
            os.system('sudo ip link add dev vcan1 type vcan')
            os.system('sudo ip link set up vcan1')
            print("Virtual CAN interface vcan1 created and set up")

class MainLayout(BoxLayout):
    def __init__(self, **kwargs):
        super(MainLayout, self).__init__(**kwargs)
        self.orientation = 'vertical'
        self.padding = 10
        self.spacing = 10

        self.dropdown = DropDown()
        sequences = ["request_ids", "update_to"]
        for seq in sequences:
            btn = Button(text=seq, size_hint_y=None, height=30, width=200)
            btn.bind(on_release=lambda btn: self.dropdown.select(btn.text))
            self.dropdown.add_widget(btn)

        self.main_button = Button(text='Select Sequence', size_hint=(None, None), size=(200, 30), pos_hint={'x': 0})
        self.main_button.bind(on_release=self.dropdown.open)
        self.dropdown.bind(on_select=self.on_select_sequence)

        self.add_widget(self.main_button)

        self.frame_id_input = TextInput(hint_text='Enter Frame ID (e.g., 0xFA)', multiline=False)
        self.add_widget(self.frame_id_input)

        self.frame_data_input = TextInput(hint_text='Enter Frame Data (comma separated, e.g., 0x02,0x99)', multiline=False)
        self.add_widget(self.frame_data_input)

        self.output_label = Label(text='Output will be shown here.', size_hint=(1, 0.6))
        self.add_widget(self.output_label)

        self.execute_button = Button(text='Execute', size_hint=(None, None), size=(200, 30), pos_hint={'x': 0})
        self.execute_button.bind(on_release=self.execute_sequence)
        self.add_widget(self.execute_button)

        self.selected_sequence = None
        self.frame_requester = FrameRequester('vcan1')

    def on_select_sequence(self, instance, value):
        self.main_button.text = value
        self.selected_sequence = value
        arbitration_id, data = self.frame_requester.get_last_frame_for_service(value)
        if arbitration_id is not None and data is not None:
            self.frame_id_input.text = f"0x{arbitration_id:X}"
            self.frame_data_input.text = ','.join([f"0x{x:02X}" for x in data])
        else:
            self.frame_id_input.text = ''
            self.frame_data_input.text = ''

    def execute_sequence(self, instance):
        frame_id = self.frame_id_input.text
        frame_data = self.frame_data_input.text

        try:
            arbitration_id = int(frame_id, 16)
            data = [int(x, 16) for x in frame_data.split(',')]
        except ValueError:
            self.output_label.text = "Invalid frame ID or data format. Please use hex values."
            return

        if self.selected_sequence == "request_ids":
            output = self.frame_requester.send_request_frame(self.selected_sequence, arbitration_id, data)
            output += "\n" + self.frame_requester.read_response_frames(self.selected_sequence)
        elif self.selected_sequence == "update_to":
            version = 1234  # example version number
            self.output_label.text = "Starting update sequence..."
            Clock.schedule_once(lambda dt: self.run_update_sequence(version, data), 0)
        else:
            output = "No sequence selected."
        self.output_label.text = output

    def run_update_sequence(self, version, data):
        try:
            result = self.frame_requester.update_to(version, data)
            self.output_label.text += f"\n{result}"
        except Exception as e:
            self.output_label.text += f"\nError: {e}"

    def on_stop(self):
        self.frame_requester.close()

if __name__ == '__main__':
    MainApp().run()
